# Pixel Gun 3D: PC Edition - Internal Cheat

![Cheat Version](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fraw.githubusercontent.com%2FRiiK26%2FPixelGun3D-Cheetos%2Fmain%2Fvcpkg.json&query=%24.version&label=Version&color=green)
![Game Version](https://img.shields.io/badge/Game%20Version-26.12.0.0-brightgreen.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
[![Build Status](https://github.com/RiiK26/PixelGun3D-Cheetos/actions/workflows/build.yml/badge.svg)](https://github.com/RiiK26/PixelGun3D-Cheetos/actions)

## Features
</details>

<details>
<summary><b>Visuals</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Player ESP** | Box, Health, Name | <span title="Work">✅</span> |
| **Skeleton ESP** | Draws player skeletons |  <span title="Work">✅</span>|

</details>

<details>
<summary><b>Weapon Mods</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Infinite Ammo** | Unlimited ammunition | <span title="Work">✅</span> |
| **Guaranteed Criticals** | 100% Crit Chance with adjustable multiplier | <span title="Work">✅</span> |
| **Auto Headshot** | Automatically headshot | <span title="Work">✅</span> |
| **AOE Bullets** | Area of Effect with adjustable radius | <span title="Work">✅</span> |

</details>

<details>
<summary><b>Player Mods</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **God Mode** | Invicible | <span title="Work">✅</span> |
| **Auto Heal** | Automatically regenerates health | <span title="Work">✅</span> |
| **High Jump** | Adjustable jump multiplier | <span title="Work">✅</span> |
| **Fly** | Enable fly mode | <span title="Work">✅</span> |

</details>

<details>
<summary><b>Currency</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Lottery** | Modify output chest lottery event | <span title="Work">✅</span> |
| **Multiple Coin** | Multiple coins from match rewards | <span title="Work">✅</span> |
| **Free Store** | Open any item in the store for free | <span title="Work">✅</span> |

</details>

<details>
<summary><b>System</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Bypass Anti-Cheat** | Enabled by default | <span title="Work">✅</span> |

</details>

## How to Build

### Prerequisites
- **CMake** (3.20 or higher)
- **MinGW-w64** (for cross-compiling on Linux or building natively)
- **vcpkg** (The project includes it, just make sure it's initialized)

### Build Steps (cross-compilation)

1. **Clone the repository (with submodules):**
   ```bash
   git clone --recursive https://github.com/RiiK26/PixelGun3D-Cheetos.git
   cd PixelGun3D-Cheetos
   ```
   *(If `vcpkg` isn't bootstrapped yet, run `./vcpkg/bootstrap-vcpkg.sh` first)*

2. **Configure the project:**
   Generate the build files using CMake and configure vcpkg to use the MinGW static triplet:
   ```bash
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-static -DCMAKE_BUILD_TYPE=Release
   ```

3. **Compile the code:**
   ```bash
   cmake --build build -j$(nproc)
   ```

4. **Locate the Output:**
   Once successfully built, the resulting `trainer.dll` and `PG3D_Injector.exe` will be located inside the `build/bin/` directory.

## How to Run

Whether you compiled the code yourself or downloaded a pre-built Release archive, we provide smart launch scripts to make injecting easy:

**On Windows:**
1. Start the game via Steam.
2. Click `runWindows.bat` (if downloaded from Releases) or `scripts\runWindows.bat` (if built from source).

**Linux or macOS (Proton/Wine):**
1. Start the game via Steam.
2. Open a terminal and run `./runLinux.sh` (if downloaded from Releases) or `./scripts/runLinux.sh` (if built from source).
*(Note: This requires `protontricks` installed on your system to correctly inject into the Steam Proton prefix).*

## Support

If you like this project and want to support its development, consider buying me a coffee or donating!

<details>
<summary><b>Donation Addresses</b></summary>

| Payment Method | Address |
| :--- | :--- |
| [![PayPal](https://img.shields.io/badge/PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.com/paypalme/MuhamadSyakir) | [Click Me!](https://www.paypal.com/paypalme/MuhamadSyakir) |
| ![Bitcoin](https://img.shields.io/badge/Bitcoin-000000?style=for-the-badge&logo=bitcoin&logoColor=white) | `bc1qpp50c2wuz5n2rq9jy3fxdmte7smcwu5rnegu6q` |
| ![Ethereum](https://img.shields.io/badge/Ethereum-3C3C3D?style=for-the-badge&logo=ethereum&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![Tether](https://img.shields.io/badge/Tether-14B693?style=for-the-badge&logo=tether&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![BNB](https://img.shields.io/badge/BNB-F3BA2F?style=for-the-badge&logo=binance&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![XRP](https://img.shields.io/badge/XRP-23292F?style=for-the-badge&logo=xrp&logoColor=white) | `rnDnG9QBce7sbmY86HXqBUpcCq6LN3xfXg` |
| ![USDC](https://img.shields.io/badge/USDC-2775CA?style=for-the-badge&logo=usd-coin&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![Solana](https://img.shields.io/badge/Solana-000000?style=for-the-badge&logo=solana&logoColor=blue) | `FLFVbCaYQWoPrm9rH1WLuoYmiVnC5CFWoCPzkNk2vzy2` |
| ![Tron](https://img.shields.io/badge/Tron-FF0013?style=for-the-badge&logo=tron&logoColor=white) | `TYNLxxQWERit64uNo8dSQX3CxdLmomtAq7` |
| ![Dogecoin](https://img.shields.io/badge/Dogecoin-C2A633?style=for-the-badge&logo=dogecoin&logoColor=white) | `DFwJXDQsqPEdpnWhXCMmBWMru9iqYqWhDn` |
| ![Cardano](https://img.shields.io/badge/Cardano-0033AD?style=for-the-badge&logo=cardano&logoColor=white) | `addr1qyhapduj2uvu8x4ct75hujtsx63uq375cxda8m25xgn7wep06zmey4cecwdtshaf0eyhqd4rcprafsvm60k4gv38uajq2z2l6a` |
| ![Litecoin](https://img.shields.io/badge/Litecoin-A6A9AA?style=for-the-badge&logo=litecoin&logoColor=black) | `LMNbzEJ3M4qtAxsYyxMBkT4rzjSBJvUc2M` |
| ![Avalanche](https://img.shields.io/badge/Avalanche-E84142?style=for-the-badge&logo=avalanche&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![Polkadot](https://img.shields.io/badge/Polkadot-E6007A?style=for-the-badge&logo=polkadot&logoColor=white) | `15FtRzNuwpbNAjvMzRCegqDwN7cgw2a44ogfobdS7UmNACFr` |
| ![Polygon](https://img.shields.io/badge/Polygon-8247E5?style=for-the-badge&logo=polygon&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![Cosmos](https://img.shields.io/badge/Cosmos-2E3148?style=for-the-badge&logo=cosmos&logoColor=white) | `cosmos1ashcczkgj884t9gert2se4m4zw5ea7t2fa4qh9` |
| ![X0 Cash](https://img.shields.io/badge/X0_Cash-228B22?style=for-the-badge&logoColor=white) | `FLFVbCaYQWoPrm9rH1WLuoYmiVnC5CFWoCPzkNk2vzy2` |

</details>

## DISCLAIMER
> **Warning**: Using cheats in online games may violate the game's terms of service and could result in account suspension or banning. I'm not responsible for any consequences arising from its use.

## LICENSE
This project under [MIT LICENSE](license)
