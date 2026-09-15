#include "playermod.h"
#include "../../utils/il2cpp.h"
#include "../../ui/menu.h"

namespace PlayerMod
{
  void Tick()
  {
    try {
      if (Settings::bAutoHeal)
        ApplyAutoHeal();

      if (Settings::bHighJump)
        ApplyHighJump();

      ApplyFly();
      ApplySpeedHack();
    } catch (...) {
      // Prevent crash
    }
  }

  void InitMenu()
  {
    Menu::AddMenuItem({"-- PLAYER --", Menu::ItemType::Header});
    Menu::AddMenuItem({"God Mode", Menu::ItemType::Bool, &Settings::bGodMode});
    Menu::AddMenuItem({"Auto Heal", Menu::ItemType::Bool, &Settings::bAutoHeal});
    Menu::AddMenuItem({"High Jump", Menu::ItemType::Bool, &Settings::bHighJump});
    Menu::AddMenuItem(
      {"Jump Multiplier", Menu::ItemType::Float, nullptr, &Settings::fJumpMultiplier, 1.0f, 10.0f, 0.5f}
    );
    Menu::AddMenuItem({"Speed Hack", Menu::ItemType::Bool, &Settings::bSpeedHack});
    Menu::AddMenuItem({"Speed Multiplier", Menu::ItemType::Float, nullptr, &Settings::fSpeedHack, 1.1f, 10.0f, 0.5f});
    Menu::AddMenuItem({"Fly", Menu::ItemType::Bool, &Settings::bFly});
    Menu::AddMenuItem({"Fly Speed", Menu::ItemType::Float, nullptr, &Settings::fFlySpeed, 0.1f, 2.0f, 0.1f});
  }
}  // namespace PlayerMod
