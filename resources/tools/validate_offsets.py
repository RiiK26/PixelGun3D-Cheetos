#!/usr/bin/env python3
"""
Validate / update src/utils/offsets.h against resources/dumped/dump.cs.

Resolution order for every `constexpr uintptr_t X = 0x..;` in offsets.h:
  1. Exact name (after MAPPINGS) in the mapped class.
  2. Case-insensitive name match.
  3. Type hint: the trailing comment type (e.g. `// PlayerDamageable*`) or an
     explicit TYPE_HINTS entry. If exactly ONE field of that type exists in the
     class, it is used (handles obfuscated field names).
  4. Otherwise -> [MISSING]. We never assume an old value is still valid just
     because *some* field still lives at that offset (that hid broken offsets).

Usage:
  validate_offsets.py            # dry run
  validate_offsets.py --update   # write changes to offsets.h
Exit code is 1 if anything is missing or ambiguous.
"""
import re
import sys
import os

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
DUMP_FILE = os.path.join(PROJECT_ROOT, "resources", "dumped", "dump.cs")
OFFSETS_FILE = os.path.join(PROJECT_ROOT, "src", "utils", "offsets.h")
IL2CPP_FILE = os.path.join(PROJECT_ROOT, "src", "utils", "il2cpp.cpp")

# Map old class names to new ones if they changed
CLASS_MAPPINGS = {
    "ItemPrice": "一世丄与东丐丟世丅",
    "ClanStoreItemData": "下丕下东丛丕七三丝"
}

# Map variable names to their exact dump.cs names
MAPPINGS = {
    # AntiCheat
    "Trigger": "丗不丟七不丌丐丐丅",
    "ShowBanner": "不下丗丟下丆丕丄丌",
    "LotteryDropCount": "get_Count",

    # MatchReward
    "ShowResultCoroutine": "丙万丈一丕三丛专丏",

    # WeaponSounds
    "sectorsAOEDmgMultFront": "sectorsAOEDamageMultiplierFront",
    "sectorsAOEDmgMultSide": "sectorsAOEDamageMultiplierSide",
    "sectorsAOEDmgMultBack": "sectorsAOEDamageMultiplierBack",
    "sectorsAOERadius": "sectorsAOERadiusSectorsAoE",

    # Store
    "get_Price": "丝丙丛业丕上丟不与",
    "get_Currency": "丂与丁丙上丁丐丒业",
}

# Explicit type hints for fields whose names are obfuscated.
# "Namespace::var" -> (field type as in dump.cs, is_static[, ordinal])
# Special type "@self" means "the class's own type" (singletons).
# ordinal picks the Nth field of that type in declaration order (stable
# across updates) when several fields share the type.
TYPE_HINTS = {
    "WeaponManager::StaticInstance": ("@self", True),
    "PlayerMoveC::playerDamageable": ("PlayerDamageable", False),
    "PlayerMoveC::visibleObjRef": ("visibleObjPhoton", False),
    "PlayerMoveC::weaponSoundsRef": ("WeaponSounds", False, 0),
    "PlayerDamageable::playerMoveC": ("Player_move_c", False),
}

# Overload selection: "Namespace::var_RVA" -> parameter count
METHOD_ARGC = {
	"Object::FindObjectsOfType_RVA": 2,
}

IGNORED_NAMESPACES = {
    "IL2CPPStructs": "Internal IL2CPP struct",
    "LiveWeapon": "Dynamic resolution fallback",
}

CLASS_RE = re.compile(r'\bclass\s+([\w\.`<>]+?)\s*(?::.*)?//\s*TypeDefIndex:\s*\d+')
FIELD_RE = re.compile(
    r'^\s*(?:\[[^\]]*\]\s*)?((?:(?:public|private|protected|internal|static|readonly|const|new|volatile)\s+)+)'
    r'(.+?)\s+([^\s;]+);\s*//\s*(0x[0-9a-fA-F]+)')
RVA_RE = re.compile(r'//\s*RVA:\s*(0x[0-9a-fA-F]+)')
METHOD_RE = re.compile(r'([^\s(]+)\s*\((.*)\)')


def count_params(params):
    params = params.strip()
    if not params:
        return 0
    depth, n = 0, 1
    for ch in params:
        if ch in "<[(":
            depth += 1
        elif ch in ">])":
            depth -= 1
        elif ch == "," and depth == 0:
            n += 1
    return n


def parse_dump():
    classes = {}
    cur = None
    pending_rva = None
    with open(DUMP_FILE, "r", encoding="utf-8") as f:
        for line in f:
            m = CLASS_RE.search(line)
            if m:
                # Keep the FULL name (incl. "Outer.Inner") so nested classes
                # don't get merged into their parent.
                cur = classes.setdefault(m.group(1), {"fields": [], "methods": {}})
                pending_rva = None
                continue
            if cur is None:
                continue

            m = RVA_RE.search(line)
            if m and "|-RVA" not in line:
                pending_rva = m.group(1)
                continue

            if pending_rva:
                m = METHOD_RE.search(line)
                if m:
                    cur["methods"].setdefault(m.group(1), []).append(
                        {"rva": pending_rva, "argc": count_params(m.group(2))})
                pending_rva = None
                continue

            m = FIELD_RE.search(line)
            if m:
                mods, ftype, name, off = m.groups()
                if "const " in mods:
                    continue
                cur["fields"].append({
                    "name": name, "type": ftype.strip(),
                    "static": "static" in mods, "offset": off,
                })
    return classes


def norm_type(t):
    return t.strip().rstrip("*").strip()


def resolve_field(cls_name, cls, ns, var, search_name, comment):
    fields = cls["fields"]
    # 1/2. by name
    for f in fields:
        if f["name"] == search_name:
            return f["offset"], f"name '{f['name']}'"
    for f in fields:
        if f["name"].lower() == search_name.lower():
            return f["offset"], f"name '{f['name']}' (ci)"

    # 3. by type
    hint = TYPE_HINTS.get(f"{ns}::{var}")
    if hint is None and comment:
        tm = re.match(r'\s*([\w\.<>]+)\*', comment)  # "// Foo*"  => reference type
        if tm:
            hint = (tm.group(1), False)
    if hint is None:
        return None, "no name match and no type hint"

    want, want_static = hint[0], hint[1]
    ordinal = hint[2] if len(hint) > 2 else None
    if want == "@self":
        want = cls_name.split(".")[-1]
    cands = [f for f in fields if norm_type(f["type"]) == want and f["static"] == want_static]
    if ordinal is not None and ordinal < len(cands):
        c = cands[ordinal]
        return c["offset"], f"type '{want}' #{ordinal} -> '{c['name']}'"
    if len(cands) == 1:
        return cands[0]["offset"], f"type '{want}' -> '{cands[0]['name']}'"
    if not cands:
        return None, f"no field of type '{want}'"
    names = ", ".join(f"{c['name']}@{c['offset']}" for c in cands)
    return None, f"AMBIGUOUS type '{want}': {names} (add ordinal to TYPE_HINTS)"


def resolve_method(cls, tag, search, old_hex):
    methods = cls["methods"]
    overloads = methods.get(search)
    if not overloads:
        key = next((k for k in methods if k.lower() == search.lower()), None)
        overloads = methods.get(key) if key else None
    if not overloads:
        # Hint: who owns the old RVA now? (name probably re-obfuscated)
        owner = next((k for k, ol in methods.items()
                      for o in ol if int(o["rva"], 16) == int(old_hex, 16)), None)
        hint = f"; old RVA belongs to '{owner}' (update MAPPINGS?)" if owner else ""
        return None, f"method '{search}' not found{hint}"

    argc = METHOD_ARGC.get(tag)
    if argc is not None:
        sel = [o for o in overloads if o["argc"] == argc]
    elif len(overloads) > 1:
        # Keep the current one if it is still one of the overloads
        sel = [o for o in overloads if int(o["rva"], 16) == int(old_hex, 16)] or overloads
    else:
        sel = overloads
    if len(sel) != 1:
        rvas = ", ".join(f"{o['rva']}({o['argc']} args)" for o in overloads)
        return None, f"AMBIGUOUS overloads of '{search}': {rvas} (add METHOD_ARGC)"
    return sel[0]["rva"], f"method '{search}'/{sel[0]['argc']}"


def run_update():
    if not os.path.exists(DUMP_FILE):
        print(f"Error: dump.cs not found at {DUMP_FILE}")
        sys.exit(1)

    print("=== Parsing dump.cs ===")
    classes = parse_dump()
    print(f"Parsed {len(classes)} classes from dump.cs.")

    print("\n=== Updating offsets.h ===")
    with open(OFFSETS_FILE, "r", encoding="utf-8") as f:
        lines = f.readlines()

    # namespace -> dump class, from "// ClassName (TypeDefIndex: N)" headers
    ns_to_class = {}
    last_class = None
    for line in lines:
        m = re.search(r'//\s+([\w_\.]+)\s*\(TypeDefIndex', line)
        if m:
            last_class = m.group(1).replace("UnityEngine.", "")
            continue
        m = re.match(r'\s*namespace\s+(\w+)', line)
        if m and last_class:
            if m.group(1) != "Offsets":
                ns_to_class[m.group(1)] = CLASS_MAPPINGS.get(last_class, last_class)
            last_class = None

    current_ns = ""
    out = []
    updates = missing = 0
    var_re = re.compile(r'^(\s*constexpr\s+uintptr_t\s+(\w+)\s*=\s*)(0x[0-9a-fA-F]+)(;\s*(?://\s*(.*))?)$')

    for line in lines:
        m = re.match(r'\s*namespace\s+(\w+)', line)
        if m:
            current_ns = m.group(1)
            out.append(line)
            continue
        if "}" in line and "namespace" in line:
            current_ns = ""

        m = var_re.match(line.rstrip("\r\n"))
        if not m:
            out.append(line)
            continue

        prefix, var, old_hex, suffix, comment = m.groups()
        tag = f"{current_ns}::{var}"

        if current_ns in IGNORED_NAMESPACES:
            print(f"[IGNORED] {tag} ({IGNORED_NAMESPACES[current_ns]})")
            out.append(line)
            continue
        if current_ns not in ns_to_class:
            print(f"[MISSING] {tag}: no '(TypeDefIndex)' header for namespace {current_ns}")
            missing += 1
            out.append(line)
            continue

        cls_name = ns_to_class[current_ns]
        cls = classes.get(cls_name)
        if cls is None:
            print(f"[MISSING] {tag}: class '{cls_name}' not in dump (update CLASS_MAPPINGS)")
            missing += 1
            out.append(line)
            continue

        is_method = var.endswith("_RVA")
        search = var[:-4] if is_method else var
        if is_method and current_ns == "AntiCheat" and search.startswith("CBD_"):
            search = search[4:]
        search = MAPPINGS.get(search, search)

        if is_method:
            new_hex, how = resolve_method(cls, tag, search, old_hex)
        else:
            new_hex, how = resolve_field(cls_name, cls, current_ns, var, search, comment)

        if not new_hex:
            print(f"[MISSING] {tag} ({cls_name}): {how}")
            missing += 1
        elif int(new_hex, 16) != int(old_hex, 16):
            print(f"[UPDATED] {tag}: {old_hex} -> {new_hex}  via {how}")
            le = "\r\n" if line.endswith("\r\n") else "\n"
            line = f"{prefix}{new_hex}{suffix}{le}"
            updates += 1
        else:
            print(f"[OK] {tag} = {old_hex}  via {how}")
        out.append(line)

    if "--update" in sys.argv or "update" in sys.argv:
        with open(OFFSETS_FILE, "w", encoding="utf-8", newline="") as f:
            f.writelines(out)
        print("\n[INFO] Applied updates to offsets.h")
    else:
        print("\n[INFO] Dry run. Use --update to write changes to offsets.h")

    print("\n=== Validating dynamic offsets in il2cpp.cpp ===")
    if os.path.exists(IL2CPP_FILE):
        with open(IL2CPP_FILE, "r", encoding="utf-8") as f:
            content = f.read()
        var_to_class = {}
        for b in re.finditer(r'namespace\s+Offsets\s*\{\s*namespace\s+(\w+)\s*\{([\s\S]*?)\}\s*\}', content):
            for v in re.finditer(r'(\w+)\s*=\s*IL2CPP::ResolveFieldOffset', b.group(2)):
                var_to_class[v.group(1)] = ns_to_class.get(b.group(1))

        for m in re.finditer(r'(\w+)\s*=\s*IL2CPP::ResolveFieldOffset\([^,]+,\s*\{([^}]+)\}', content):
            var = m.group(1)
            names = re.findall(r'"([^"]+)"', m.group(2))
            target = var_to_class.get(var)
            pool = [classes.get(target, {"fields": []})] if target else classes.values()
            hit = next((n for n in names for c in pool if any(f["name"] == n for f in c["fields"])), None)
            if hit:
                print(f"[OK] Dynamic {target or '?'}::{var} via '{hit}'")
            else:
                print(f"[MISSING] Dynamic {target or '?'}::{var} (tried {names}) - add obfuscated name in il2cpp.cpp")
                missing += 1

    print("\n==================================")
    print(f"{updates} updated, {missing} missing/ambiguous.")
    sys.exit(1 if missing else 0)


if __name__ == "__main__":
    run_update()
