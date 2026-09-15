#include "hooks.h"
#include <MinHook.h>
#include "il2cpp.h"
#include "../features/visual/visual.h"
#include "../features/playermod/playermod.h"
#include "../features/weaponmod/weaponmod.h"
#include "../features/currency/currency.h"
#include "../utils/config.h"
#include "../ui/menu.h"
#include <string>

namespace Hooks
{
  fn_ApplyDamage oApplyDamage = nullptr;

  // ---- Lottery: Drop Count Hook ----
  fn_LotteryDropCount oLotteryDropCount = nullptr;

  int32_t hkLotteryDropCount(void* arg)
  {
    // Safety: validate arg before calling original
    if (!arg) {
      if (oLotteryDropCount)
        return oLotteryDropCount(arg);
      return 0;
    }

    return CurrencyMod::OnLotteryDropCount(oLotteryDropCount(arg));
  }

  // ---- Store: Item Price Hook ----
  fn_ItemPriceGetCurrency oItemPriceGetCurrency = nullptr;
  fn_ItemPriceGetPrice    oItemPriceGetPrice    = nullptr;
  int32_t hkItemPriceGetPrice(void* thisPtr)
  {
    if (CurrencyMod::Settings::bFreeStore)
      return 0;

    return oItemPriceGetPrice(thisPtr);
  }

  using fn_StoreItemDataGetPrice                  = void* (*) (void* thisPtr);
  fn_StoreItemDataGetPrice oStoreItemDataGetPrice = nullptr;

  void* hkStoreItemDataGetPrice(void* thisPtr)
  {
    void* listObj = oStoreItemDataGetPrice(thisPtr);

    if (CurrencyMod::Settings::bFreeStore && listObj) {
      IL2CPP::WriteFieldIfValid<int>(listObj, Offsets::IL2CPPStructs::listSizeOffset, 0);
    }

    return listObj;
  }

  // ---- Match Reward: ShowResult Coroutine Hook ----
  fn_ShowResultCoroutine oShowResultCoroutine = nullptr;

  void* hkShowResultCoroutine(
    void* thisPtr,
    void* winner,
    void* ratingChange,
    bool  showAward,
    int   realAddCoin,  // Param 4: Discovered to be the real coin amount!
    int   blueTotal,
    bool  firstPlace,
    bool  deadheatDuel,
    bool  param8,
    bool  iAmWinnerInTeam,
    int   realAddExp,  // Param 10: Discovered to be the real EXP amount!
    int   addEventCurrency,
    int   winnerCommand,
    int   bpCurrency,
    int   pixelPassCurrency,
    bool  param15,
    int   pixelPassExp,
    int   param17,
    int   param18,
    int   param19,
    bool  clanCurrencyLimitReached,
    void* addDetails,
    void* addModuleChest,
    int   winterPoints,
    void* vipRewards,
    int   springPt1Points,
    int   springPt2Points,
    int   springPt2Currency,
    int   gemsByHarvester,
    int   balanceBrawlPoints,
    int   addGems,
    void* templateEventItems,
    void* methodInfo
  )
  {
    if (CurrencyMod::Settings::bEnableRewardMultiplier) {
      // Multiply only the real coins parameter (param 4)
      realAddCoin = static_cast<int>(realAddCoin * CurrencyMod::Settings::fCoinsMultiplier);
    }

    return oShowResultCoroutine(
      thisPtr, winner, ratingChange, showAward, realAddCoin, blueTotal, firstPlace, deadheatDuel, param8,
      iAmWinnerInTeam, realAddExp, addEventCurrency, winnerCommand, bpCurrency, pixelPassCurrency, param15,
      pixelPassExp, param17, param18, param19, clanCurrencyLimitReached, addDetails, addModuleChest, winterPoints,
      vipRewards, springPt1Points, springPt2Points, springPt2Currency, gemsByHarvester, balanceBrawlPoints, addGems,
      templateEventItems, methodInfo
    );
  }

  // ---- ApplyDamage Hook ----
  void hkApplyDamage(
    void*       thisPtr,
    float       damage,
    void*       attacker,
    void*       collider,
    void*       hitPoint,
    int         damageType,
    int         typeDead,
    const char* weaponId,
    int         someInt,
    void*       someEnum,
    void*       methodInfo
  )
  {
    if (PlayerMod::OnApplyDamage(thisPtr)) {
      return;
    }

    WeaponMod::ApplyAutoHeadshot(thisPtr, damageType);

    oApplyDamage(
      thisPtr, damage, attacker, collider, hitPoint, damageType, typeDead, weaponId, someInt, someEnum, methodInfo
    );
  }

  using fn_OnEventFired         = bool (*)(void* thisPtr, uint8_t eventCode, void* eventData, void* methodInfo);
  fn_OnEventFired oOnEventFired = nullptr;

  bool hkOnEventFired(void* thisPtr, uint8_t eventCode, void* eventData, void* methodInfo)
  {
    if (PlayerMod::OnEventFired(thisPtr, eventCode)) {
      return true;
    }
    return oOnEventFired(thisPtr, eventCode, eventData, methodInfo);
  }

  using fn_PlayerMoveC_Update               = void (*)(void* thisPtr);
  fn_PlayerMoveC_Update oPlayerMoveC_Update = nullptr;

  void hkPlayerMoveC_Update(void* thisPtr)
  {
    // Apply our mods BEFORE the game's Update so our values are ready when the game processes shooting
    bool isLocalPlayer = false;
    if (thisPtr) {
      Offsets::PlayerMoveC::InitDynamicOffsets();
      Offsets::SkinName::InitDynamicOffsets();

      if (Offsets::PlayerMoveC::mySkinNameOffset > 0 && Offsets::SkinName::isMineOffset > 0) {
        void* skinName = IL2CPP::SafeReadField<void*>(thisPtr, Offsets::PlayerMoveC::mySkinNameOffset);
        if (skinName) {
          isLocalPlayer = IL2CPP::SafeReadField<bool>(skinName, Offsets::SkinName::isMineOffset);
        }
      }
      else {
        isLocalPlayer = (thisPtr == IL2CPP::GetLocalPlayerMoveC());
      }
    }

    if (isLocalPlayer) {
      Visual::TickMainThread();
      WeaponMod::Tick();
      PlayerMod::Tick();
    }

    // Run the game's original Update
    if (oPlayerMoveC_Update) {
      oPlayerMoveC_Update(thisPtr);
    }
  }

  fn_CBD_Trigger oCBD_Trigger = nullptr;
  void hkCBD_Trigger() { /* NOP */ }

  fn_CBD_Show oCBD_Show = nullptr;
  void hkCBD_Show() { /* NOP */ }


  fn_Present oPresent = nullptr;

  bool Init()
  {
    if (MH_Initialize() != MH_OK)
      return false;

    auto gaBase = IL2CPP::gameAssemblyBase;

    // 1. Hook ApplyDamage for no fall damage
    void* pApplyDamage = reinterpret_cast<void*>(gaBase + Offsets::PlayerDamageable::ApplyDamage_RVA);
    if (MH_CreateHook(pApplyDamage, (void*) &hkApplyDamage, reinterpret_cast<void**>(&oApplyDamage)) != MH_OK) {
      return false;
    }
    MH_EnableHook(pApplyDamage);

    // 2. Hook OnEventFired to block network damage RPCs for God Mode
    void* pOnEventFired = reinterpret_cast<void*>(gaBase + Offsets::PlayerMoveC::OnEventFired_RVA);
    if (MH_CreateHook(pOnEventFired, (void*) &hkOnEventFired, reinterpret_cast<void**>(&oOnEventFired)) != MH_OK) {
      return false;
    }
    MH_EnableHook(pOnEventFired);


    auto cbdTriggerAddr = reinterpret_cast<void*>(gaBase + Offsets::AntiCheat::CBD_Trigger_RVA);
    MH_CreateHook(cbdTriggerAddr, reinterpret_cast<void*>(&hkCBD_Trigger), reinterpret_cast<void**>(&oCBD_Trigger));

    auto cbdShowAddr = reinterpret_cast<void*>(gaBase + Offsets::AntiCheat::CBD_ShowBanner_RVA);
    MH_CreateHook(cbdShowAddr, reinterpret_cast<void*>(&hkCBD_Show), reinterpret_cast<void**>(&oCBD_Show));

    // 3. Hook the drop count getter
    auto lotteryDropCountAddr = reinterpret_cast<void*>(gaBase + Offsets::Lottery::LotteryDropCount_RVA);
    MH_CreateHook(
      lotteryDropCountAddr, reinterpret_cast<void*>(&hkLotteryDropCount), reinterpret_cast<void**>(&oLotteryDropCount)
    );

    // Store
    auto itemPriceGetCurrencyAddr = reinterpret_cast<void*>(gaBase + Offsets::ItemPrice::get_Currency_RVA);
    MH_CreateHook(
      itemPriceGetCurrencyAddr, nullptr, reinterpret_cast<void**>(&oItemPriceGetCurrency)
    );  // We only need the original function ptr to call it

    auto itemPriceGetPriceAddr = reinterpret_cast<void*>(gaBase + Offsets::ItemPrice::get_Price_RVA);
    if (
      MH_CreateHook(
        itemPriceGetPriceAddr, reinterpret_cast<void*>(&hkItemPriceGetPrice),
        reinterpret_cast<void**>(&oItemPriceGetPrice)
      )
      != MH_OK
    ) {
      return false;
    }

    auto storeItemPriceAddr = reinterpret_cast<void*>(gaBase + Offsets::StoreItemData::get_Price_RVA);
    if (
      MH_CreateHook(
        storeItemPriceAddr, reinterpret_cast<void*>(&hkStoreItemDataGetPrice),
        reinterpret_cast<void**>(&oStoreItemDataGetPrice)
      )
      != MH_OK
    )
      return false;

    // 5. Match Reward: Hook ShowResult coroutine
    auto showResultAddr = reinterpret_cast<void*>(gaBase + Offsets::MatchReward::ShowResultCoroutine_RVA);
    MH_CreateHook(
      showResultAddr, reinterpret_cast<void*>(&hkShowResultCoroutine), reinterpret_cast<void**>(&oShowResultCoroutine)
    );

    // Dynamic resolution for Player_move_c::Update
    void* updateMethod = IL2CPP::class_get_method_from_name((void*) Offsets::Classes::PlayerMoveC, "Update", 0);
    if (updateMethod) {
      void* updatePointer = *reinterpret_cast<void**>(updateMethod);
      MH_CreateHook(
        updatePointer, reinterpret_cast<void*>(&hkPlayerMoveC_Update), reinterpret_cast<void**>(&oPlayerMoveC_Update)
      );
    }

    MH_EnableHook(MH_ALL_HOOKS);

    return true;
  }

  void SetupMenu()
  {
    Menu::AddMenuItem({"-- SETTINGS --", Menu::ItemType::Header, false});
    Menu::AddMenuItem({"Bypass Anti-Cheat", Menu::ItemType::Bool, &Settings::bAntiCheatBypass});
    Menu::AddMenuItem(
      {"Save Config", Menu::ItemType::Action, nullptr, nullptr, nullptr, 0.0f, 0.0f, 0.0f, 0, 0, 0,
       []() { Config::Save(); }}
    );
    Menu::AddMenuItem(
      {"Load Config", Menu::ItemType::Action, nullptr, nullptr, nullptr, 0.0f, 0.0f, 0.0f, 0, 0, 0,
       []() { Config::Load(); }}
    );
  }

  void Shutdown()
  {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
  }
}  // namespace Hooks
