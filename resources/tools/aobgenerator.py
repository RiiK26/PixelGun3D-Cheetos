#!/usr/bin/env python3
"""
AOB signature tool for GameAssembly.dll.

Every `constexpr uintptr_t Foo_RVA = 0x...;` in src/utils/offsets.h can have a
companion `constexpr const char* Foo_SIG = "48 89 5C 24 ?? ...";`.
At runtime IL2CPP::ResolveMethod() scans for Foo_SIG and only falls back to
Foo_RVA when the signature is empty or not unique.

Commands:
  aobgenerator.py generate [--dll PATH] [--update]
      Build a unique signature for each *_RVA from the CURRENT (matching) dll.
      With --update, write/refresh the *_SIG lines in offsets.h.

  aobgenerator.py scan [--dll PATH] [--update]
      After a game update: locate every *_SIG in the NEW dll and report RVAs.
      With --update, rewrite the *_RVA fallbacks in offsets.h.

Wildcarded operands (they change between builds):
  - RIP-relative displacements (globals, metadata, string literals)
  - rel8/rel32 branch / call targets
  - memory displacements off non-stack registers (object field offsets
    like [rcx+0x740] / [rax+20h]) - stack frame offsets are kept
"""
import os
import re
import sys
import glob
import argparse
import platform

import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_MEM, CS_OP_IMM
from capstone.x86 import X86_REG_RIP, X86_REG_RSP, X86_REG_RBP

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
OFFSETS_FILE = os.path.join(PROJECT_ROOT, "src", "utils", "offsets.h")

MIN_SIG_BYTES = 16       # never emit shorter signatures
MAX_SIG_BYTES = 192      # give up after this many bytes
BRANCH_GROUPS = {"jump", "call", "branch_relative"}


def find_unity_dll(app_id="2524890", is_il2cpp=True):
    steam_paths = []
    if platform.system() == 'Windows':
        try:
            import winreg
            with winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\Valve\Steam") as key:
                steam_path = winreg.QueryValueEx(key, "SteamPath")[0]
                steam_paths.append(steam_path)
        except Exception:
            pass
        for p in [r"C:\Program Files (x86)\Steam", r"C:\Program Files\Steam"]:
            if os.path.exists(p) and p not in steam_paths:
                steam_paths.append(p)
    elif platform.system() == 'Linux':
        for p in [os.path.expanduser("~/.steam/steam"), os.path.expanduser("~/.local/share/Steam")]:
            if os.path.exists(p):
                steam_paths.append(p)
    elif platform.system() == 'Darwin':
        p = os.path.expanduser("~/Library/Application Support/Steam")
        if os.path.exists(p):
            steam_paths.append(p)

    library_folders = []
    for sp in steam_paths:
        if sp not in library_folders:
            library_folders.append(sp)
        vdf_path = os.path.join(sp, "steamapps", "libraryfolders.vdf")
        if os.path.exists(vdf_path):
            try:
                with open(vdf_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    paths = re.findall(r'"path"\s+"([^"]+)"', content)
                    for p in paths:
                        clean_path = p.replace('\\\\', '\\')
                        if clean_path not in library_folders:
                            library_folders.append(clean_path)
            except Exception:
                pass

    for lib in library_folders:
        manifest_path = os.path.join(lib, "steamapps", f"appmanifest_{app_id}.acf")
        if os.path.exists(manifest_path):
            try:
                with open(manifest_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    match = re.search(r'"installdir"\s+"([^"]+)"', content, re.IGNORECASE)
                    if match:
                        install_dir = match.group(1)
                        game_path = os.path.join(lib, "steamapps", "common", install_dir)

                        if is_il2cpp:
                            # IL2CPP
                            dll_path = os.path.join(game_path, "GameAssembly.dll")
                        else:
                            # Mono
                            dll_path = None
                            data_folders = glob.glob(os.path.join(game_path, "*_Data"))
                            if data_folders:
                                dll_path = os.path.join(data_folders[0], "Managed", "Assembly-CSharp.dll")

                        if dll_path and os.path.exists(dll_path):
                            print(f"[+] Found DLL via AppID {app_id} at: {dll_path}")
                            return dll_path
            except Exception as e:
                print(f"[-] Error reading manifest: {e}")

    target_name = "GameAssembly.dll" if is_il2cpp else "Assembly-CSharp.dll"
    fallback_path = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))), target_name)
    print(f"[-] Could not find game in Steam libraries. Falling back to: {fallback_path}")
    return fallback_path


# --------------------------------------------------------------------------
# PE image helpers
# --------------------------------------------------------------------------
class Image:
    def __init__(self, path):
        print(f"[*] Loading {path}")
        self.pe = pefile.PE(path, fast_load=True)
        self.data = self.pe.__data__
        # Executable sections: (rva_start, rva_end, raw_start, bytes)
        self.code = []
        for s in self.pe.sections:
            if s.Characteristics & 0x20000000:  # IMAGE_SCN_MEM_EXECUTE
                raw = bytes(s.get_data())
                size = min(len(raw), max(s.Misc_VirtualSize, 1))
                self.code.append((s.VirtualAddress, s.VirtualAddress + size, raw[:size]))
        if not self.code:
            sys.exit("[-] No executable section found")

    def read(self, rva, size):
        for start, end, raw in self.code:
            if start <= rva < end:
                off = rva - start
                return raw[off:off + size]
        return None

    def find(self, regex, limit=2):
        """Return up to `limit` RVAs matching the compiled bytes regex."""
        hits = []
        for start, _, raw in self.code:
            for m in regex.finditer(raw):
                hits.append(start + m.start())
                if len(hits) >= limit:
                    return hits
        return hits


def to_regex(sig):
    parts = []
    for tok in sig.split():
        parts.append(b"." if tok in ("?", "??") else re.escape(bytes([int(tok, 16)])))
    return re.compile(b"".join(parts), re.DOTALL)


def fmt(mask_bytes):
    return " ".join("??" if b is None else f"{b:02X}" for b in mask_bytes)


# --------------------------------------------------------------------------
# Signature generation
# --------------------------------------------------------------------------
def masked_insn_bytes(insn):
    out = list(insn.bytes)

    def wipe(off, size):
        for i in range(off, min(off + size, len(out))):
            out[i] = None

    is_branch = any(insn.group_name(g) in BRANCH_GROUPS for g in insn.groups)
    for op in insn.operands:
        if op.type == CS_OP_MEM and insn.disp_size:
            if op.mem.base == X86_REG_RIP or op.mem.base not in (X86_REG_RSP, X86_REG_RBP) \
                    or insn.disp_size >= 4:
                wipe(insn.disp_offset, insn.disp_size)
        elif op.type == CS_OP_IMM and is_branch and insn.imm_size:
            wipe(insn.imm_offset, insn.imm_size)
    # movabs / imm64 pointers
    if insn.imm_size == 8:
        wipe(insn.imm_offset, 8)
    return out


def generate_sig(img, md, rva):
    code = img.read(rva, MAX_SIG_BYTES + 16)
    if not code:
        return None, "RVA not in an executable section"

    sig = []
    for insn in md.disasm(code, rva):
        sig.extend(masked_insn_bytes(insn))
        concrete = sum(b is not None for b in sig)
        if len(sig) >= MIN_SIG_BYTES and concrete >= 8:
            hits = img.find(to_regex(fmt(sig)))
            if hits == [rva]:
                # Trim trailing wildcards
                while sig and sig[-1] is None:
                    sig.pop()
                return fmt(sig), None
        if len(sig) >= MAX_SIG_BYTES:
            break
        if insn.mnemonic in ("ret", "int3"):
            # End of function body: anything after this is unrelated code
            return None, f"not unique within function body ({len(sig)} bytes) - tiny/shared stub?"
    return None, f"not unique within {MAX_SIG_BYTES} bytes"


# --------------------------------------------------------------------------
# offsets.h I/O
# --------------------------------------------------------------------------
RVA_LINE = re.compile(r'^(\s*)constexpr\s+uintptr_t\s+(\w+)_RVA\s*=\s*(0x[0-9a-fA-F]+)\s*;')
SIG_LINE = re.compile(r'^(\s*)constexpr\s+const\s+char\s*\*\s*(\w+)_SIG\s*=\s*"([^"]*)"\s*;')
NS_LINE = re.compile(r'^\s*namespace\s+(\w+)')


def load_offsets():
    with open(OFFSETS_FILE, "r", encoding="utf-8", newline="") as f:
        return f.readlines()


def save_offsets(lines):
    with open(OFFSETS_FILE, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"[+] Wrote {OFFSETS_FILE}")


def iter_entries(lines):
    """Yield (ns, name, rva_line_idx, rva, sig_line_idx|None, sig)."""
    ns = ""
    for i, line in enumerate(lines):
        m = NS_LINE.match(line)
        if m and m.group(1) != "Offsets":
            ns = m.group(1)
        m = RVA_LINE.match(line)
        if not m:
            continue
        name = m.group(2)
        sig_idx, sig = None, ""
        for j in range(i + 1, min(i + 4, len(lines))):
            s = SIG_LINE.match(lines[j])
            if s and s.group(2) == name:
                sig_idx, sig = j, s.group(3)
                break
        yield ns, name, i, int(m.group(3), 16), sig_idx, sig


# --------------------------------------------------------------------------
# Commands
# --------------------------------------------------------------------------
def cmd_generate(args):
    img = Image(args.dll)
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True

    lines = load_offsets()
    entries = list(iter_entries(lines))
    ok = fail = 0
    # Walk backwards so inserted lines don't shift later indices
    for ns, name, i, rva, sig_idx, _ in reversed(entries):
        sig, err = generate_sig(img, md, rva)
        tag = f"{ns}::{name}"
        if not sig:
            print(f"[FAIL] {tag} @ {rva:#x}: {err}")
            fail += 1
            sig = ""
        else:
            print(f"[OK]   {tag} @ {rva:#x}: {sig}")
            ok += 1
        indent = RVA_LINE.match(lines[i]).group(1)
        le = "\r\n" if lines[i].endswith("\r\n") else "\n"
        new_line = f'{indent}constexpr const char* {name}_SIG = "{sig}";{le}'
        if sig_idx is not None:
            lines[sig_idx] = new_line
        else:
            lines.insert(i + 1, new_line)

    print(f"\n{ok} generated, {fail} failed (failed ones fall back to *_RVA at runtime)")
    if args.update:
        save_offsets(lines)
    else:
        print("[INFO] Dry run. Use --update to write *_SIG lines to offsets.h")
    return 1 if fail else 0


def cmd_scan(args):
    img = Image(args.dll)
    lines = load_offsets()
    changed = missing = 0
    for ns, name, i, rva, _, sig in iter_entries(lines):
        tag = f"{ns}::{name}"
        if not sig:
            print(f"[SKIP] {tag}: no signature (keep validating {name}_RVA via validate_offsets.py)")
            continue
        hits = img.find(to_regex(sig), limit=3)
        if len(hits) != 1:
            print(f"[FAIL] {tag}: {len(hits)} matches - regenerate signature")
            missing += 1
            continue
        new = hits[0]
        if new == rva:
            print(f"[OK]   {tag} = {rva:#x}")
            continue
        print(f"[MOVED] {tag}: {rva:#x} -> {new:#x}")
        lines[i] = re.sub(r'0x[0-9a-fA-F]+', f"{new:#X}".replace("0X", "0x"), lines[i], count=1)
        changed += 1

    print(f"\n{changed} moved, {missing} broken")
    if args.update and changed:
        save_offsets(lines)
    elif changed:
        print("[INFO] Dry run. Use --update to rewrite *_RVA fallbacks in offsets.h")
    return 1 if missing else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("command", choices=["generate", "scan"])
    ap.add_argument("--dll", help="Path to GameAssembly.dll (auto-detected from Steam if omitted)")
    ap.add_argument("--update", action="store_true", help="Write changes to offsets.h")
    args = ap.parse_args()
    args.dll = args.dll or find_unity_dll(app_id="2524890", is_il2cpp=True)
    if not os.path.exists(args.dll):
        sys.exit(f"[-] GameAssembly.dll not found: {args.dll} (pass --dll)")
    sys.exit(cmd_generate(args) if args.command == "generate" else cmd_scan(args))


if __name__ == "__main__":
    main()
