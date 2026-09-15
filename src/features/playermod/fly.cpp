#include "playermod.h"
#include "../../utils/il2cpp.h"
#include <windows.h>

namespace PlayerMod
{
  void ApplyFly()
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

    void* myTransform = IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::myPlayerTransform);
    if (!myTransform)
      return;

    static float originalVelocityDownFallMultiplier = 1.0f;
    static bool  wasFlying                          = false;
    static float lockedY                            = 0.0f;
    static void* lastPmc                            = nullptr;

    if (pmc != lastPmc) {
      wasFlying = false;
      lastPmc   = pmc;
    }

    Offsets::FirstPersonControlSharp::InitDynamicOffsets();

    if (Settings::bFly) {
      if (!wasFlying) {
        // Save original gravity multiplier
        originalVelocityDownFallMultiplier = IL2CPP::SafeReadField<float>(
          firstPersonControl, Offsets::FirstPersonControlSharp::velocityDownFallMultiplierOffset
        );
        lockedY   = IL2CPP::GetTransformPosition(myTransform).y;
        wasFlying = true;
      }

      // Disable gravity pull by zeroing out downfall multiplier
      IL2CPP::WriteField<float>(
        firstPersonControl, Offsets::FirstPersonControlSharp::velocityDownFallMultiplierOffset, 0.0f
      );

      // Handle smooth vertical movement
      if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        lockedY += Settings::fFlySpeed;
      }
      else if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
        lockedY -= Settings::fFlySpeed;
      }

      // Apply locked position (smooth fly, zero jitter)
      auto pos = IL2CPP::GetTransformPosition(myTransform);
      IL2CPP::SetTransformPosition(myTransform, pos.x, lockedY, pos.z);
    }
    else {
      if (wasFlying) {
        // Restore original gravity multiplier when fly is disabled
        IL2CPP::WriteField<float>(
          firstPersonControl, Offsets::FirstPersonControlSharp::velocityDownFallMultiplierOffset,
          originalVelocityDownFallMultiplier
        );
        wasFlying = false;
      }
    }
  }
}  // namespace PlayerMod
