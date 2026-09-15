#include "weaponmod.h"
#include "../../utils/il2cpp.h"

namespace WeaponMod
{
  void ApplyInfiniteAmmo(void* ws)
  {
    if (!ws)
      return;

    using namespace Offsets::WeaponSounds;
    Offsets::WeaponSounds::InitDynamicOffsets();
    IL2CPP::WriteField(ws, isUnlimitedAmmoOffset, true);

    int initAmmo = IL2CPP::ReadField<int>(ws, Offsets::WeaponSounds::InitialAmmo);

    // Update live active weapon instance
    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (pmc) {
      void* charWeaponCurrent = IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::charWeaponCurrent);
      if (charWeaponCurrent) {
        void* charWeaponClass = IL2CPP::object_get_class(charWeaponCurrent);
        if (charWeaponClass) {
          Offsets::LiveWeapon::InitDynamicOffsets(charWeaponClass);

          // Freeze spare ammo so it doesn't decrease when reloading
          if (initAmmo > 0) {
            IL2CPP::WriteField(charWeaponCurrent, Offsets::LiveWeapon::liveAmmoOffset, initAmmo);
          }
        }
      }
    }
  }
}  // namespace WeaponMod
