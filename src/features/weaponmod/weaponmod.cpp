#include "weaponmod.h"
#include "../../utils/il2cpp.h"
#include "../../ui/menu.h"

namespace WeaponMod
{
  WeaponModBackup backup           = {};
  void*           lastWeaponSounds = nullptr;

  void BackupWeaponSounds(void* ws)
  {
    if (!ws || ws == lastWeaponSounds)
      return;
    lastWeaponSounds = ws;

    using namespace Offsets::WeaponSounds;

    Offsets::WeaponSounds::InitDynamicOffsets();

    // Ammo
    backup.isUnlimitedAmmo = IL2CPP::ReadField<bool>(ws, isUnlimitedAmmoOffset);
    backup.ammoInClip      = IL2CPP::ReadField<int>(ws, Offsets::WeaponSounds::ammoInClip);

    // Crit
    backup.criticalHitChance = IL2CPP::ReadField<int>(ws, criticalHitChance);
    backup.criticalHitCoef   = IL2CPP::ReadField<float>(ws, criticalHitCoef);


    // AOE Bullets
    backup.isSectorsAOE           = IL2CPP::ReadField<bool>(ws, isSectorsAOE);
    backup.sectorsAOEAngleFront   = IL2CPP::ReadField<float>(ws, sectorsAOEAngleFront);
    backup.sectorsAOEAngleBack    = IL2CPP::ReadField<float>(ws, sectorsAOEAngleBack);
    backup.sectorsAOERadius       = IL2CPP::ReadField<float>(ws, sectorsAOERadius);
    backup.sectorsAOEDmgMultFront = IL2CPP::ReadField<float>(ws, sectorsAOEDmgMultFront);
    backup.sectorsAOEDmgMultSide  = IL2CPP::ReadField<float>(ws, sectorsAOEDmgMultSide);
    backup.sectorsAOEDmgMultBack  = IL2CPP::ReadField<float>(ws, sectorsAOEDmgMultBack);
    backup.bazookaExplosionRadius = IL2CPP::ReadField<float>(ws, bazookaExplosionRadius);

    backup.hasBackup              = true;
  }

  void RestoreWeaponSounds(void* ws)
  {
    if (!ws || !backup.hasBackup)
      return;

    using namespace Offsets::WeaponSounds;

    Offsets::WeaponSounds::InitDynamicOffsets();

    // Ammo
    IL2CPP::WriteField(ws, isUnlimitedAmmoOffset, backup.isUnlimitedAmmo);
    IL2CPP::WriteField(ws, Offsets::WeaponSounds::ammoInClip, backup.ammoInClip);

    // Crit
    IL2CPP::WriteField(ws, criticalHitChance, backup.criticalHitChance);
    IL2CPP::WriteField(ws, criticalHitCoef, backup.criticalHitCoef);

    // AOE Bullets
    IL2CPP::WriteField(ws, isSectorsAOE, backup.isSectorsAOE);
    IL2CPP::WriteField(ws, sectorsAOEAngleFront, backup.sectorsAOEAngleFront);
    IL2CPP::WriteField(ws, sectorsAOEAngleBack, backup.sectorsAOEAngleBack);
    IL2CPP::WriteField(ws, sectorsAOERadius, backup.sectorsAOERadius);
    IL2CPP::WriteField(ws, sectorsAOEDmgMultFront, backup.sectorsAOEDmgMultFront);
    IL2CPP::WriteField(ws, sectorsAOEDmgMultSide, backup.sectorsAOEDmgMultSide);
    IL2CPP::WriteField(ws, sectorsAOEDmgMultBack, backup.sectorsAOEDmgMultBack);
    IL2CPP::WriteField(ws, bazookaExplosionRadius, backup.bazookaExplosionRadius);
  }

  void Tick()
  {
    try {
      auto ws = IL2CPP::GetCurrentWeaponSounds();
      if (!ws)
        return;

      BackupWeaponSounds(ws);

      if (backup.hasBackup) {
        RestoreWeaponSounds(ws);
      }

      if (Settings::bInfiniteAmmo)
        ApplyInfiniteAmmo(ws);
      if (Settings::bCritChance100)
        ApplyCritChance(ws);

      if (Settings::bAOEBullets)
        ApplyAOEBullets(ws);

      bool anyActive = Settings::bInfiniteAmmo || Settings::bCritChance100 || Settings::bAOEBullets;

      if (!anyActive) {
        backup.hasBackup = false;
        lastWeaponSounds = nullptr;
      }
    } catch (...) {
      // Prevent crash
    }
  }

  void InitMenu()
  {
    Menu::AddMenuItem({"-- WEAPON --", Menu::ItemType::Header});
    Menu::AddMenuItem({"Infinite Ammo", Menu::ItemType::Bool, &Settings::bInfiniteAmmo});
    Menu::AddMenuItem({"100% Crit Chance", Menu::ItemType::Bool, &Settings::bCritChance100});
    Menu::AddMenuItem(
      {"Crit Multiplier", Menu::ItemType::Float, nullptr, &Settings::fCritMultiplier, 1.0f, 20.0f, 1.0f}
    );
    Menu::AddMenuItem({"Auto Headshot", Menu::ItemType::Bool, &Settings::bAutoHeadshot});
    Menu::AddMenuItem({"AOE Bullets", Menu::ItemType::Bool, &Settings::bAOEBullets});
    Menu::AddMenuItem({"AOE Radius", Menu::ItemType::Float, nullptr, &Settings::fAOERadius, 5.0f, 200.0f, 5.0f});
  }
}  // namespace WeaponMod
