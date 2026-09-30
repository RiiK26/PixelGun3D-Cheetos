#!/usr/bin/env python3

import os
import re
import json
import pefile
import platform
import glob
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_MEM, CS_OP_IMM

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

GAME_ASSEMBLY_DLL_PATH = find_unity_dll(app_id="2524890", is_il2cpp=True)
