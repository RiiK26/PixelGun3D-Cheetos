#include "currency.h"
#include "../../ui/menu.h"

namespace CurrencyMod
{
  void InitMenu()
  {
    Menu::AddMenuItem({"-- CURRENCY --", Menu::ItemType::Header});

    // Lottery
    Menu::AddMenuItem({"Modify Lottery Output", Menu::ItemType::Bool, &Settings::bModifyChestOutput});
    Menu::AddMenuItem(
      {"Lottery Output Amount", Menu::ItemType::Int, nullptr, nullptr, &Settings::iChestOutputAmount, 0.0f, 0.0f, 0.0f,
       0, 100000, 50}
    );

    // Free Store
    Menu::AddMenuItem({"Free Store (0 everything)", Menu::ItemType::Bool, &Settings::bFreeStore});

    // Match Reward
    Menu::AddMenuItem({"Enable Reward Multiplier", Menu::ItemType::Bool, &Settings::bEnableRewardMultiplier});
    Menu::AddMenuItem(
      {"Coins Multiplier", Menu::ItemType::Float, nullptr, &Settings::fCoinsMultiplier, 1.0f, 100000.0f, 100.0f}
    );
  }
}  // namespace CurrencyMod
