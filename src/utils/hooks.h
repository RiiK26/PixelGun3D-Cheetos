#pragma once
#include <windows.h>
#include <cstdint>

namespace Hooks
{
  namespace Settings
  {
    inline bool bAntiCheatBypass = true;
  }

  void SetupMenu();

  // ---- ApplyDamage Hook (God Mode) ----
  using fn_ApplyDamage = void (*)(
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
  );
  extern fn_ApplyDamage oApplyDamage;

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
  );

  // ---- MinusLive Hook (God Mode alternative) ----
  using fn_MinusLive = void (*)(void* thisPtr, float damage, bool isHeadshot, int weaponType, void* methodInfo);
  extern fn_MinusLive oMinusLive;

  void hkMinusLive(void* thisPtr, float damage, bool isHeadshot, int weaponType, void* methodInfo);

  // ---- MinusLiveReal Hook ----
  using fn_MinusLiveReal = void (*)(void* thisPtr, float damage, bool isHeadshot, int weaponType, void* methodInfo);
  extern fn_MinusLiveReal oMinusLiveReal;

  void hkMinusLiveReal(void* thisPtr, float damage, bool isHeadshot, int weaponType, void* methodInfo);

  // ---- CheatDetectedBanner Bypass ----
  using fn_CBD_Trigger = void (*)();
  extern fn_CBD_Trigger oCBD_Trigger;
  void hkCBD_Trigger();

  using fn_CBD_Show = void (*)();
  extern fn_CBD_Show oCBD_Show;
  void hkCBD_Show();

  // ---- Lottery: Drop Count Hook (Chest Outputs) ----
  using fn_LotteryDropCount = int32_t (*)(void* arg);
  extern fn_LotteryDropCount oLotteryDropCount;

  int32_t hkLotteryDropCount(void* arg);

  // ---- Store: Item Price Hook ----
  using fn_ItemPriceGetCurrency = void* (*) (void* thisPtr);
  extern fn_ItemPriceGetCurrency oItemPriceGetCurrency;

  using fn_ItemPriceGetPrice = int32_t (*)(void* thisPtr);
  extern fn_ItemPriceGetPrice oItemPriceGetPrice;

  int32_t hkItemPriceGetPrice(void* thisPtr);

  // ---- Match Reward: ShowResult Coroutine ----
  // NetworkStartTableNGUIController.Obf_B4BEBDFB — 31 parameters
  // We hook this to multiply exp and coins before the coroutine processes them
  using fn_ShowResultCoroutine = void* (*)(
    void* thisPtr,
    void* Obf_9306E8D0,        // string: winner name
    void* Obf_C4D0C293,        // RatingSystem.Obf_B3291474: rating change (struct)
    bool  Obf_CCA82BCF,        // showAward
    int   Obf_D0C8EF01,        // _addExpierence
    int   Obf_EAC58016,        // blueTotal
    bool  Obf_8C48FC7E,        // firstPlace
    bool  Obf_4E16CED8,        // deadheatDuel
    bool  Obf_D3767BDD,        // ?
    bool  Obf_BF566EAF,        // iAmWinnerInTeam
    int   Obf_AE517117,        // _addCoin
    int   Obf_651A8EB7,        // _addEventCurrency
    int   Obf_EBF8C39C,        // _winnerCommand
    int   Obf_2A904AB4,        // _bpCurrency
    int   Obf_E6B4EAA1,        // _pixelPassCurrency
    bool  Obf_E1F6A213,        // ?
    int   Obf_97F90896,        // _pixelPassExp
    int   Obf_78D18262,        // ?
    int   Obf_D8A2E591,        // ?
    int   Obf_B6239BF4,        // ?
    bool  Obf_3FBB0D8C,        // _clanCurrencyLimitReached
    void* Obf_09021AA8,        // OfferItem _addDetails
    void* Obf_1BDFB071,        // Obf_C3E5263B.Obf_5B5C1B32: _addModuleChest
    int   Obf_FC4697ED,        // _winterEvent2023LeaderboardPoints
    void* Obf_2A914B96,        // List<Obf_0AFB13CD> vipRewards
    int   Obf_5971786D,        // _springEvent2023PartOneLeaderboardPoints
    int   Obf_019C8547,        // _springEvent2023PartTwoLeaderboardPoints
    int   Obf_36357B8B,        // _springEvent2023PartTwoCurrency
    int   Obf_08A89C0D,        // _gemsByGemsHarvester
    int   Obf_577B6512,        // _balanceBrawlLeaderboardPoints
    int   Obf_A7814652,        // _addGems
    void* Obf_3831FE85,        // Obf_98ACDCCA: templateEventItems
    void* methodInfo           // hidden IL2CPP MethodInfo*
  );
  extern fn_ShowResultCoroutine oShowResultCoroutine;

  void* hkShowResultCoroutine(
    void* thisPtr,
    void* Obf_9306E8D0,
    void* Obf_C4D0C293,
    bool  Obf_CCA82BCF,
    int   realAddCoin,
    int   blueTotal,
    bool  firstPlace,
    bool  deadheatDuel,
    bool  param8,
    bool  iAmWinnerInTeam,
    int   realAddExp,
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
  );

  // ---- IAP: ProcessPurchase ----
  // Obf_E9D37A5D.ProcessPurchase(PurchaseEventArgs)
  // Returns PurchaseProcessingResult: Complete=0, Pending=1
  using fn_ProcessPurchase = int32_t (*)(void* thisPtr, void* purchaseEventArgs);
  extern fn_ProcessPurchase oProcessPurchase;

  int32_t hkProcessPurchase(void* thisPtr, void* purchaseEventArgs);

  // ---- PixelPass Premium: HasPremium ----
  using fn_PPBoolGetter = bool (*)(void* thisPtr);


  // ---- D3D11 Present Hook (for ImGui rendering) ----
  using fn_Present = HRESULT(WINAPI*)(void* pSwapChain, UINT SyncInterval, UINT Flags);
  extern fn_Present oPresent;

  // Forward declare — implemented in menu.cpp or dllmain.cpp
  HRESULT WINAPI hkPresent(void* pSwapChain, UINT SyncInterval, UINT Flags);

  // ---- Initialize all hooks ----
  bool Init();
  void Shutdown();
}  // namespace Hooks
