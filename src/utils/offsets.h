#pragma once
#include <cstdint>

namespace Offsets
{
  // ==========================================
  // WeaponManager (TypeDefIndex: 5805)
  // ==========================================
  namespace WeaponManager
  {
    constexpr uintptr_t StaticInstance = 0x238;  // WeaponManager singleton
    constexpr uintptr_t myPlayerMoveC  = 0x50;   // Player_move_c*
  }  // namespace WeaponManager

  // ==========================================
  // Player_move_c (TypeDefIndex: 1241)
  // ==========================================
  namespace PlayerMoveC
  {
    constexpr uintptr_t   charWeaponCurrent   = 0xC40;      // current weapon struct
    constexpr uintptr_t   myPlayerTransform   = 0x450;      // Transform*
    constexpr uintptr_t   PlayerHeadTransform = 0x508;      // Transform*
    constexpr uintptr_t   weaponSoundsRef     = 0x718;      // WeaponSounds* (current)
    constexpr uintptr_t   playerDamageable    = 0x740;      // PlayerDamageable*
    constexpr uintptr_t   visibleObjRef       = 0x820;      // visibleObjPhoton*
    constexpr uintptr_t   nickLabel           = 0x470;      // TextMesh*
    constexpr uintptr_t   playerBodyRenderer  = 0x4E0;      // SkinnedMeshRenderer*
    constexpr uintptr_t   OnEventFired_RVA    = 0x153B640;  // In Player_move_c
    constexpr const char* OnEventFired_SIG =
      "40 55 53 56 57 48 8D 6C 24 C1 48 81 EC C8 00 00 00 80 3D ?? ?? ?? ?? 00 49 8B D8";

    // Dynamic offsets resolved at runtime
    inline size_t mySkinNameOffset       = 0;
    inline bool   dynamicOffsetsResolved = false;

    void InitDynamicOffsets();
  }  // namespace PlayerMoveC

  // ==========================================
  // WeaponSounds (TypeDefIndex: 5877)
  // ==========================================
  namespace WeaponSounds
  {
    constexpr uintptr_t ammoInClip             = 0x74;   // int
    constexpr uintptr_t InitialAmmo            = 0x78;   // int
    constexpr uintptr_t isUnlimitedAmmo        = 0x562;  // bool
    constexpr uintptr_t tekKoof                = 0xA4;   // float (base scatter)
    constexpr uintptr_t upKoofFire             = 0xA8;   // float (scatter increase on fire)
    constexpr uintptr_t downKoofFirst          = 0xAC;   // float (first shot recovery)
    constexpr uintptr_t downKoof               = 0xB0;   // float (scatter recovery)
    constexpr uintptr_t moveScatterCoeff       = 0xCC;   // float (movement scatter)
    constexpr uintptr_t firstShotScatter       = 0xC4;   // bool
    constexpr uintptr_t upKoofFireZoom         = 0x11C;  // float
    constexpr uintptr_t downKoofFirstZoom      = 0x120;  // float
    constexpr uintptr_t downKoofZoom           = 0x124;  // float
    constexpr uintptr_t recoilCoeffZoom        = 0x138;  // float
    constexpr uintptr_t firstShotScatterZoom   = 0x12C;  // bool
    constexpr uintptr_t moveScatterCoeffZoom   = 0x130;  // float
    constexpr uintptr_t isPiercingMelee        = 0x1B6;  // bool
    constexpr uintptr_t distancePiercingMelee  = 0x1B8;  // float
    constexpr uintptr_t bulletDelay            = 0x1DC;  // float
    constexpr uintptr_t shootDelay             = 0x1E4;  // float
    constexpr uintptr_t chargeTime             = 0x224;  // float
    constexpr uintptr_t bazookaExplosionRadius = 0x160;  // float
    constexpr uintptr_t bazooka                = 0x148;  // bool
    constexpr uintptr_t criticalHitChance      = 0x57C;  // int (0-100)
    constexpr uintptr_t criticalHitCoef        = 0x580;  // float (multiplier)
    constexpr uintptr_t DelayTimer             = 0x568;  // float
    constexpr uintptr_t isSectorsAOE           = 0x49C;  // bool
    constexpr uintptr_t isMelee                = 0x199;  // bool
    constexpr uintptr_t sectorsAOEAngleFront   = 0x4A0;  // float
    constexpr uintptr_t sectorsAOEAngleBack    = 0x4A4;  // float
    constexpr uintptr_t sectorsAOEDmgMultFront = 0x4A8;  // float
    constexpr uintptr_t sectorsAOEDmgMultSide  = 0x4AC;  // float
    constexpr uintptr_t sectorsAOEDmgMultBack  = 0x4B0;  // float
    constexpr uintptr_t sectorsAOERadius       = 0x4B4;  // float
    constexpr uintptr_t inShopEffects          = 0x5C8;  // List<int>

    // Dynamic offsets resolved at runtime
    inline size_t isUnlimitedAmmoOffset  = 0;
    inline size_t canAffectAlliesOffset  = 0;
    inline bool   dynamicOffsetsResolved = false;

    void InitDynamicOffsets();
  }  // namespace WeaponSounds

  // ==========================================
  // PlayerDamageable (TypeDefIndex: 1711)
  // ==========================================
  namespace PlayerDamageable
  {
    constexpr uintptr_t   playerMoveC     = 0x20;  // Player_move_c*
    constexpr uintptr_t   ApplyDamage_RVA = 0x1D6D030;
    constexpr const char* ApplyDamage_SIG = "4C 89 4C 24 20 56 41 54 41 56 48 81 EC E0 00 00 00";
    constexpr uintptr_t   IsDead_RVA      = 0x1D6E560;
    constexpr const char* IsDead_SIG      = "";
    constexpr uintptr_t   IsEnemyTo_RVA   = 0x1D6E580;
    constexpr const char* IsEnemyTo_SIG =
      "48 89 5C 24 10 57 48 83 EC 20 80 3D ?? ?? ?? ?? 00 48 8B DA 48 8B F9 75 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? "
      "48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 01 48 8B 0D ?? ?? ?? "
      "?? 83 B9 ?? ?? ?? ?? 00 75 ?? E8 ?? ?? ?? ?? 33 C9";
  }  // namespace PlayerDamageable

  // ==========================================
  // ItemRecord (TypeDefIndex: 6461)
  // ==========================================
  namespace ItemRecord
  {
    constexpr uintptr_t ammoInClip      = 0x20;
    constexpr uintptr_t isUnlimitedAmmo = 0x6C;
  }  // namespace ItemRecord

  // ==========================================
  // WeaponContainer (TypeDefIndex: 7682)
  // ==========================================
  namespace WeaponContainer
  {
    constexpr uintptr_t initialAmmo = 0x64;
  }

  // ==========================================
  // Live Weapon (Dynamic resolution fallback)
  // ==========================================
  namespace LiveWeapon
  {
    constexpr uintptr_t ammoFallback = 0x48;

    // Dynamic offsets resolved at runtime
    inline size_t liveAmmoOffset         = 0;
    inline bool   dynamicOffsetsResolved = false;

    void InitDynamicOffsets(void* charWeaponClass);
  }  // namespace LiveWeapon

  // ==========================================
  // SkinName (TypeDefIndex: 6028)
  // ==========================================
  namespace SkinName
  {
    inline size_t isMineOffset             = 0;
    inline size_t firstPersonControlOffset = 0;
    inline bool   dynamicOffsetsResolved   = false;

    void InitDynamicOffsets();
  }  // namespace SkinName

  // ==========================================
  // FirstPersonControlSharp (TypeDefIndex: 6617)
  // ==========================================
  namespace FirstPersonControlSharp
  {
    /*constexpr uintptr_t   set_MoveSpeedMultiplier_RVA = 0x8E4010;
    constexpr const char* set_MoveSpeedMultiplier_SIG = "";*/
    inline size_t velocityDownFallMultiplierOffset = 0;
    inline bool   dynamicOffsetsResolved           = false;

    void InitDynamicOffsets();
  }  // namespace FirstPersonControlSharp

  // ==========================================
  // IL2CPP Internal Structures
  // ==========================================
  namespace IL2CPPStructs
  {
    constexpr uintptr_t stringLengthOffset = 0x10;
    constexpr uintptr_t stringCharsOffset  = 0x14;
    constexpr uintptr_t arrayLengthOffset  = 0x18;
    constexpr uintptr_t arrayDataOffset    = 0x20;

    // System.Collections.Generic.List<T>
    constexpr uintptr_t listItemsOffset = 0x10;
    constexpr uintptr_t listSizeOffset  = 0x18;
  }  // namespace IL2CPPStructs

  // ==========================================
  // Object (TypeDefIndex: 12345)
  // ==========================================
  namespace Object
  {
    /*constexpr uintptr_t   FindObjectsOfType_RVA = 0x4401670;
    constexpr const char* FindObjectsOfType_SIG = "";*/
  }  // namespace Object

  // ==========================================
  // CheatDetectedBanner (TypeDefIndex: 9999)
  // ==========================================
  namespace AntiCheat
  {
    constexpr uintptr_t   CBD_Trigger_RVA = 0x214E130;  // static trigger method
    constexpr const char* CBD_Trigger_SIG =
      "48 83 EC 28 80 3D ?? ?? ?? ?? 00 75 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? "
      "48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 01 48 8B 0D ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 75 ?? E8 ?? "
      "?? ?? ?? 33 C9 E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? 83 B9 ?? ?? ?? ?? 00 75 ?? E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? "
      "?? 33 D2";
    constexpr uintptr_t   CBD_Awake_RVA  = 0x214DBA0;
    constexpr const char* CBD_Awake_SIG  = "";
    constexpr uintptr_t   CBD_Update_RVA = 0x214E1B0;
    constexpr const char* CBD_Update_SIG =
      "40 53 48 83 EC 30 80 3D ?? ?? ?? ?? 00 48 8B D9 75 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 01 "
      "80 7B ?? 00 0F 84 ?? ?? ?? ?? 80 7B ?? 00";
    constexpr uintptr_t   CBD_ShowBanner_RVA = 0x214DE50;  // static show method
    constexpr const char* CBD_ShowBanner_SIG =
      "40 53 48 83 EC 30 80 3D ?? ?? ?? ?? 00 75 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? "
      "?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? "
      "?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 01 33 C9 E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? 48 8B D8";
  }  // namespace AntiCheat

  // ==========================================
  // GameEventItemData (TypeDefIndex: 8300)
  // Lottery / Chest Pricing
  // ==========================================
  namespace Lottery
  {
    // GameEventItemData.get_Count() — returns int drop count
    constexpr uintptr_t   LotteryDropCount_RVA = 0xC41060;
    constexpr const char* LotteryDropCount_SIG =
      "40 53 48 83 EC 30 80 3D ?? ?? ?? ?? 00 48 8B D9 75 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? "
      "E8 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 01 8B 83";
  }  // namespace Lottery

  // ==========================================
  // NetworkStartTableNGUIController (TypeDefIndex: 1109)
  // Match Rewards
  // ==========================================
  namespace MatchReward
  {
    // NetworkStartTableNGUIController.丙万丈一丕三丛专丏() — ShowResult coroutine
    constexpr uintptr_t   ShowResultCoroutine_RVA = 0x13874D0;
    constexpr const char* ShowResultCoroutine_SIG =
      "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 56 48 83 EC 20 80 3D ?? ?? ?? ?? 00 45 0F B6 F1 "
      "49 8B F8 48 8B F2 48 8B E9 75 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 01 48 8B 0D ?? ?? ?? ?? "
      "E8 ?? ?? ?? ?? 45 33 C0";
  }  // namespace MatchReward

  // ==========================================
  // ItemPrice (TypeDefIndex: 6526)
  // Store - Item Price
  // ==========================================
  namespace ItemPrice
  {
    // ItemPrice (一世丄与东丐丟世丅).get_Currency()
    constexpr uintptr_t   get_Currency_RVA = 0x3D50F0;
    constexpr const char* get_Currency_SIG = "";

    // ItemPrice (一世丄与东丐丟世丅).get_Price()
    constexpr uintptr_t   get_Price_RVA = 0x83CA00;  // get_Price for "Coins", "Gems", and "Lottery keys"
    constexpr const char* get_Price_SIG = "";
  }  // namespace ItemPrice

  // ==========================================
  // ClanStoreItemData (TypeDefIndex: 13025)
  // Store - Item Data
  // ==========================================
  namespace StoreItemData
  {
    // ClanStoreItemData (下丕下东丛丕七三丝).get_Price()
    constexpr uintptr_t   get_Price_RVA = 0x17D7F70;  // returns List<与丙丙丛丂丄丛东业>
    constexpr const char* get_Price_SIG =
      "48 83 EC 28 48 8B 51 ?? 48 85 D2 74 ?? 48 8B 4A ?? 48 85 C9 74 ?? 48 8B 41 ?? 4C 8B 41 ?? 48 8B 49 ?? 8B 52 ?? "
      "FF D0 EB ?? 48 8B 42 ?? 48 85 C0 74 ?? 48 8B 40 ?? 48 85 C0 74 ?? 48 8B 40";
  }  // namespace StoreItemData

  // ==========================================
  // IL2CPP Class pointers (resolved at runtime, cached here)
  // ==========================================
  namespace Classes
  {
    inline uintptr_t WeaponManager                   = 0;
    inline uintptr_t PlayerMoveC                     = 0;
    inline uintptr_t WeaponSounds                    = 0;
    inline uintptr_t PlayerDamageable                = 0;
    inline uintptr_t CheatDetectedBanner             = 0;
    inline uintptr_t ClickerDetector                 = 0;
    inline uintptr_t SkinName                        = 0;
    inline uintptr_t FirstPersonControlSharp         = 0;
    inline uintptr_t NetworkStartTableNGUIController = 0;
  }  // namespace Classes
}  // namespace Offsets
