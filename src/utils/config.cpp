#include "config.h"
#include "../features/visual/visual.h"
#include "../features/playermod/playermod.h"
#include "../features/weaponmod/weaponmod.h"
#include "../features/visual/visual.h"
#include "../features/playermod/playermod.h"
#include "../features/weaponmod/weaponmod.h"
#include "../features/currency/currency.h"
#include "hooks.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace Config
{
  std::string configPath = "";

  void SetConfigPath(const std::string& dllPath)
  {
    // Config file next to the DLL
    size_t lastSlash = dllPath.find_last_of("\\/");
    if (lastSlash != std::string::npos)
      configPath = dllPath.substr(0, lastSlash + 1) + "pg3d_trainer.json";
    else
      configPath = "pg3d_trainer.json";
  }

  void Save()
  {
    if (configPath.empty())
      return;

    nlohmann::json j;
    j["weapon"]["aoe_bullets"]          = WeaponMod::Settings::bAOEBullets;
    j["weapon"]["aoe_radius"]           = WeaponMod::Settings::fAOERadius;

    j["visual"]["esp_boxes"]            = Visual::Settings::bPlayerESPBoxes;
    j["visual"]["esp_names"]            = Visual::Settings::bPlayerESPNames;
    j["visual"]["skeleton_esp"]         = Visual::Settings::bSkeletonESP;
    j["visual"]["treat_all_as_enemies"] = Visual::Settings::bTreatAllAsEnemies;

    // PlayerMod
    j["playermod"]["high_jump"]       = PlayerMod::Settings::bHighJump;
    j["playermod"]["jump_multiplier"] = PlayerMod::Settings::fJumpMultiplier;
    j["playermod"]["fly"]             = PlayerMod::Settings::bFly;
    j["playermod"]["god_mode"]        = PlayerMod::Settings::bGodMode;
    j["playermod"]["auto_heal"]       = PlayerMod::Settings::bAutoHeal;

    // WeaponMod
    j["weaponmod"]["infinite_ammo"]   = WeaponMod::Settings::bInfiniteAmmo;
    j["weaponmod"]["crit_chance"]     = WeaponMod::Settings::bCritChance100;
    j["weaponmod"]["crit_multiplier"] = WeaponMod::Settings::fCritMultiplier;
    j["weaponmod"]["autohs"]          = WeaponMod::Settings::bAutoHeadshot;
    j["weaponmod"]["aoe_bullets"]     = WeaponMod::Settings::bAOEBullets;
    j["weaponmod"]["aoe_radius"]      = WeaponMod::Settings::fAOERadius;

    // CurrencyMod
    j["currency"]["free_store"]               = CurrencyMod::Settings::bFreeStore;
    j["currency"]["enable_reward_multiplier"] = CurrencyMod::Settings::bEnableRewardMultiplier;
    j["currency"]["coins_multiplier"]         = CurrencyMod::Settings::fCoinsMultiplier;

    j["lottery"]["modify_output"]             = CurrencyMod::Settings::bModifyChestOutput;
    j["lottery"]["output_amount"]             = CurrencyMod::Settings::iChestOutputAmount;


    // System
    j["system"]["anticheat_bypass"] = Hooks::Settings::bAntiCheatBypass;

    std::ofstream file(configPath);
    if (file.is_open()) {
      file << j.dump(4);
      file.close();
    }
  }

  void Load()
  {
    if (configPath.empty())
      return;

    std::ifstream file(configPath);
    if (!file.is_open())
      return;

    nlohmann::json j;
    try {
      file >> j;
    } catch (...) {
      return;
    }
    file.close();

#define LOAD_BOOL(section, key, var) \
  if (j.contains(section) && j[section].contains(key)) \
  var = j[section][key].get<bool>()
#define LOAD_FLOAT(section, key, var) \
  if (j.contains(section) && j[section].contains(key)) \
  var = j[section][key].get<float>()
#define LOAD_INT(section, key, var) \
  if (j.contains(section) && j[section].contains(key)) \
  var = j[section][key].get<int>()

    LOAD_BOOL("weapon", "aoe_bullets", WeaponMod::Settings::bAOEBullets);
    LOAD_FLOAT("weapon", "aoe_radius", WeaponMod::Settings::fAOERadius);

    // Visual
    LOAD_BOOL("visual", "esp_boxes", Visual::Settings::bPlayerESPBoxes);
    LOAD_BOOL("visual", "esp_names", Visual::Settings::bPlayerESPNames);
    LOAD_BOOL("visual", "skeleton_esp", Visual::Settings::bSkeletonESP);
    LOAD_BOOL("visual", "treat_all_as_enemies", Visual::Settings::bTreatAllAsEnemies);

    // PlayerMod
    LOAD_BOOL("playermod", "high_jump", PlayerMod::Settings::bHighJump);
    LOAD_FLOAT("playermod", "jump_multiplier", PlayerMod::Settings::fJumpMultiplier);
    LOAD_BOOL("playermod", "fly", PlayerMod::Settings::bFly);
    LOAD_BOOL("playermod", "god_mode", PlayerMod::Settings::bGodMode);
    LOAD_BOOL("playermod", "auto_heal", PlayerMod::Settings::bAutoHeal);

    // WeaponMod
    LOAD_BOOL("weaponmod", "infinite_ammo", WeaponMod::Settings::bInfiniteAmmo);
    LOAD_BOOL("weaponmod", "crit_chance", WeaponMod::Settings::bCritChance100);
    LOAD_FLOAT("weaponmod", "crit_multiplier", WeaponMod::Settings::fCritMultiplier);
    LOAD_BOOL("weaponmod", "autohs", WeaponMod::Settings::bAutoHeadshot);
    LOAD_BOOL("weaponmod", "aoe_bullets", WeaponMod::Settings::bAOEBullets);
    LOAD_FLOAT("weaponmod", "aoe_radius", WeaponMod::Settings::fAOERadius);

    // CurrencyMod
    LOAD_BOOL("currency", "free_store", CurrencyMod::Settings::bFreeStore);
    LOAD_BOOL("currency", "enable_reward_multiplier", CurrencyMod::Settings::bEnableRewardMultiplier);
    LOAD_FLOAT("currency", "coins_multiplier", CurrencyMod::Settings::fCoinsMultiplier);

    LOAD_BOOL("lottery", "modify_output", CurrencyMod::Settings::bModifyChestOutput);
    LOAD_INT("lottery", "output_amount", CurrencyMod::Settings::iChestOutputAmount);

    // System
    LOAD_BOOL("system", "anticheat_bypass", Hooks::Settings::bAntiCheatBypass);

#undef LOAD_BOOL
#undef LOAD_FLOAT
#undef LOAD_INT
  }
}  // namespace Config
