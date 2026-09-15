#pragma once
#include <cstdint>

namespace Offsets
{
  // ==========================================
  // WeaponManager (TypeDefIndex: 5805)
  // ==========================================
  namespace WeaponManager
  {
    constexpr uintptr_t StaticInstance = 0x230;  // WeaponManager singleton
    constexpr uintptr_t myPlayerMoveC  = 0x48;   // Player_move_c*
  }  // namespace WeaponManager

  // ==========================================
  // Player_move_c (TypeDefIndex: 1241)
  // ==========================================
  namespace PlayerMoveC
  {
    constexpr uintptr_t charWeaponCurrent   = 0xBE8;      // current weapon struct
    constexpr uintptr_t myPlayerTransform   = 0x3F8;      // Transform*
    constexpr uintptr_t PlayerHeadTransform = 0x4B0;      // Transform*
    constexpr uintptr_t weaponSoundsRef     = 0x6C0;      // WeaponSounds* (current)
    constexpr uintptr_t playerDamageable    = 0x6E8;      // PlayerDamageable*
    constexpr uintptr_t visibleObjRef       = 0x7C8;      // visibleObjPhoton*
    constexpr uintptr_t nickLabel           = 0x418;      // TextMesh*
    constexpr uintptr_t playerBodyRenderer  = 0x488;      // SkinnedMeshRenderer*
    constexpr uintptr_t OnEventFired_RVA    = 0x162C940;  // In Player_move_c

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
    constexpr uintptr_t ammoInClip             = 0x6C;   // int
    constexpr uintptr_t InitialAmmo            = 0x70;   // int
    constexpr uintptr_t isUnlimitedAmmo        = 0x55A;  // bool
    constexpr uintptr_t tekKoof                = 0x9C;   // float (base scatter)
    constexpr uintptr_t upKoofFire             = 0xA0;   // float (scatter increase on fire)
    constexpr uintptr_t downKoofFirst          = 0xA4;   // float (first shot recovery)
    constexpr uintptr_t downKoof               = 0xA8;   // float (scatter recovery)
    constexpr uintptr_t moveScatterCoeff       = 0xC4;   // float (movement scatter)
    constexpr uintptr_t firstShotScatter       = 0xBC;   // bool
    constexpr uintptr_t upKoofFireZoom         = 0x114;  // float
    constexpr uintptr_t downKoofFirstZoom      = 0x118;  // float
    constexpr uintptr_t downKoofZoom           = 0x11C;  // float
    constexpr uintptr_t recoilCoeffZoom        = 0x130;  // float
    constexpr uintptr_t firstShotScatterZoom   = 0x124;  // bool
    constexpr uintptr_t moveScatterCoeffZoom   = 0x128;  // float
    constexpr uintptr_t isPiercingMelee        = 0x1AE;  // bool
    constexpr uintptr_t distancePiercingMelee  = 0x1B0;  // float
    constexpr uintptr_t bulletDelay            = 0x1D4;  // float
    constexpr uintptr_t shootDelay             = 0x1DC;  // float
    constexpr uintptr_t chargeTime             = 0x21C;  // float
    constexpr uintptr_t bazookaExplosionRadius = 0x158;  // float
    constexpr uintptr_t bazooka                = 0x140;  // bool
    constexpr uintptr_t criticalHitChance      = 0x574;  // int (0-100)
    constexpr uintptr_t criticalHitCoef        = 0x578;  // float (multiplier)
    constexpr uintptr_t DelayTimer             = 0x560;  // float
    constexpr uintptr_t isSectorsAOE           = 0x494;  // bool
    constexpr uintptr_t isMelee                = 0x191;  // bool
    constexpr uintptr_t sectorsAOEAngleFront   = 0x498;  // float
    constexpr uintptr_t sectorsAOEAngleBack    = 0x49C;  // float
    constexpr uintptr_t sectorsAOEDmgMultFront = 0x4A0;  // float
    constexpr uintptr_t sectorsAOEDmgMultSide  = 0x4A4;  // float
    constexpr uintptr_t sectorsAOEDmgMultBack  = 0x4A8;  // float
    constexpr uintptr_t sectorsAOERadius       = 0x4AC;  // float
    constexpr uintptr_t inShopEffects          = 0x5C0;  // List<int>

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
    constexpr uintptr_t playerMoveC     = 0x18;  // Player_move_c*
    constexpr uintptr_t ApplyDamage_RVA = 0x1DDAEC0;
    constexpr uintptr_t IsDead_RVA      = 0x1DDC3F0;
    constexpr uintptr_t IsEnemyTo_RVA   = 0x1DDC410;
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
    constexpr uintptr_t initialAmmo = 0x5C;
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
    constexpr uintptr_t set_MoveSpeedMultiplier_RVA = 0x9A8B50;

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
    constexpr uintptr_t FindObjectsOfType_RVA = 0x47B5A90;
  }

  // ==========================================
  // CheatDetectedBanner (TypeDefIndex: 9999)
  // ==========================================
  namespace AntiCheat
  {
    constexpr uintptr_t CBD_Trigger_RVA    = 0x21B1160;  // static trigger method
    constexpr uintptr_t CBD_Awake_RVA      = 0x21B0C30;
    constexpr uintptr_t CBD_Update_RVA     = 0x21B11E0;
    constexpr uintptr_t CBD_ShowBanner_RVA = 0x21B0EA0;  // static show method
  }  // namespace AntiCheat

  // ==========================================
  // GameEventItemData (TypeDefIndex: 8300)
  // Lottery / Chest Pricing
  // ==========================================
  namespace Lottery
  {
    // GameEventItemData.get_Count() — returns int drop count
    constexpr uintptr_t LotteryDropCount_RVA = 0xCFB580;
  }  // namespace Lottery

  // ==========================================
  // NetworkStartTableNGUIController (TypeDefIndex: 1110)
  // Match Rewards
  // ==========================================
  namespace MatchReward
  {
    // NetworkStartTableNGUIController.Obf_B4BEBDFB() — ShowResult coroutine
    constexpr uintptr_t ShowResultCoroutine_RVA = 0x1461350;

    // NetworkStartTableNGUIController.Obf_763E52DA() — ApplyMatchReward (Actual value applier)
    constexpr uintptr_t ApplyMatchReward_RVA = 0x145A150;
  }  // namespace MatchReward

  // ==========================================
  // ItemPrice (TypeDefIndex: 6465)
  // Store - Item Price
  // ==========================================
  namespace ItemPrice
  {
    // ItemPrice.get_Currency()
    constexpr uintptr_t get_Currency_RVA = 0x496100;

    // ItemPrice.get_Price()
    constexpr uintptr_t get_Price_RVA = 0x900B70;  // get_Price for "Coins", "Gems", and "Lottery keys"
  }  // namespace ItemPrice

  // ==========================================
  // ClanStoreItemData (TypeDefIndex: 12871)
  // Store - Item Data
  // ==========================================
  namespace StoreItemData
  {
    // ClanStoreItemData (专丙丛三丒丈丅丙世).get_Price()
    constexpr uintptr_t get_Price_RVA = 0x1885FD0;  // returns List<七丙三丒丞万一丏丕>
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
