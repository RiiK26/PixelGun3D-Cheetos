#include "playermod.h"
#include "../../utils/il2cpp.h"

namespace PlayerMod
{
  void ApplySpeedHack()
  {
    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (!pmc)
      return;

    Offsets::PlayerMoveC::InitDynamicOffsets();
    void* skinName = IL2CPP::SafeReadField<void*>(pmc, Offsets::PlayerMoveC::mySkinNameOffset);
    if (!skinName)
      return;

    Offsets::SkinName::InitDynamicOffsets();
    void* firstPersonControl = IL2CPP::SafeReadField<void*>(skinName, Offsets::SkinName::firstPersonControlOffset);
    if (!firstPersonControl)
      return;

    static bool wasSpeedHack = false;

    if (Settings::bSpeedHack) {
      using fn_set_MoveSpeedMultiplier = void (*)(void*, float, void*);
      auto pSetSpeed                   = reinterpret_cast<fn_set_MoveSpeedMultiplier>(
        IL2CPP::gameAssemblyBase + Offsets::FirstPersonControlSharp::set_MoveSpeedMultiplier_RVA
      );
      if (pSetSpeed) {
        // Multiply by our custom speed hack value
        pSetSpeed(firstPersonControl, Settings::fSpeedHack, nullptr);
        wasSpeedHack = true;
      }
    }
    else {
      if (wasSpeedHack) {
        using fn_set_MoveSpeedMultiplier = void (*)(void*, float, void*);
        auto pSetSpeed                   = reinterpret_cast<fn_set_MoveSpeedMultiplier>(
          IL2CPP::gameAssemblyBase + Offsets::FirstPersonControlSharp::set_MoveSpeedMultiplier_RVA
        );
        if (pSetSpeed) {
          // Restore default speed multiplier
          pSetSpeed(firstPersonControl, 1.0f, nullptr);
          wasSpeedHack = false;
        }
      }
    }
  }
}  // namespace PlayerMod
