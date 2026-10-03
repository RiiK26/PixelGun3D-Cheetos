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
    constexpr uintptr_t charWeaponCurrent   = 0xC40;      // current weapon struct
    constexpr uintptr_t myPlayerTransform   = 0x450;      // Transform*
    constexpr uintptr_t PlayerHeadTransform = 0x508;      // Transform*
    constexpr uintptr_t weaponSoundsRef     = 0x718;      // WeaponSounds* (current)
    constexpr uintptr_t playerDamageable    = 0x740;      // PlayerDamageable*
    constexpr uintptr_t visibleObjRef       = 0x820;      // visibleObjPhoton*
    constexpr uintptr_t nickLabel           = 0x470;      // TextMesh*
    constexpr uintptr_t playerBodyRenderer  = 0x4E0;      // SkinnedMeshRenderer*
    constexpr uintptr_t OnEventFired_RVA    = 0x153B640;  // In Player_move_c

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
    constexpr uintptr_t playerMoveC     = 0x20;  // Player_move_c*
    constexpr uintptr_t ApplyDamage_RVA = 0x1D6D030;
    constexpr uintptr_t IsDead_RVA      = 0x1D6E560;
    constexpr uintptr_t IsEnemyTo_RVA   = 0x1D6E580;
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
    constexpr uintptr_t set_MoveSpeedMultiplier_RVA = 0x8E4010;

    inline size_t velocityDownFallMultiplierOffset  = 0;
    inline bool   dynamicOffsetsResolved            = false;

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
    constexpr uintptr_t FindObjectsOfType_RVA = 0x4401670;
  }

  // ==========================================
  // CheatDetectedBanner (TypeDefIndex: 9999)
  // ==========================================
  namespace AntiCheat
  {
    constexpr uintptr_t CBD_Trigger_RVA    = 0x214E130;  // static trigger method
    constexpr uintptr_t CBD_Awake_RVA      = 0x214DBA0;
    constexpr uintptr_t CBD_Update_RVA     = 0x214E1B0;
    constexpr uintptr_t CBD_ShowBanner_RVA = 0x214DE50;  // static show method
  }  // namespace AntiCheat

  // ==========================================
  // GameEventItemData (TypeDefIndex: 8300)
  // Lottery / Chest Pricing
  // ==========================================
  namespace Lottery
  {
    // GameEventItemData.get_Count() — returns int drop count
    constexpr uintptr_t LotteryDropCount_RVA = 0xC41060;
  }  // namespace Lottery

  // ==========================================
  // NetworkStartTableNGUIController (TypeDefIndex: 1109)
  // Match Rewards
  // ==========================================
  namespace MatchReward
  {
    // NetworkStartTableNGUIController.丙万丈一丕三丛专丏() — ShowResult coroutine
    constexpr uintptr_t ShowResultCoroutine_RVA = 0x13874D0;
  }  // namespace MatchReward

  // ==========================================
  // ItemPrice (TypeDefIndex: 6526)
  // Store - Item Price
  // ==========================================
  namespace ItemPrice
  {
    // ItemPrice (一世丄与东丐丟世丅).get_Currency()
    constexpr uintptr_t get_Currency_RVA = 0x3D50F0;

    // ItemPrice (一世丄与东丐丟世丅).get_Price()
    constexpr uintptr_t get_Price_RVA = 0x83CA00;  // get_Price for "Coins", "Gems", and "Lottery keys"
  }  // namespace ItemPrice

  // ==========================================
  // ClanStoreItemData (TypeDefIndex: 13025)
  // Store - Item Data
  // ==========================================
  namespace StoreItemData
  {
    // ClanStoreItemData (下丕下东丛丕七三丝).get_Price()
    constexpr uintptr_t get_Price_RVA = 0x17D7F70;  // returns List<与丙丙丛丂丄丛东业>
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
