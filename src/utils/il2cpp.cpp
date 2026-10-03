#include "il2cpp.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace IL2CPP
{
  // Function pointer typedefs
  using fn_il2cpp_domain_get                  = void* (*) ();
  using fn_il2cpp_domain_get_assemblies       = void** (*) (void* domain, size_t* size);
  using fn_il2cpp_assembly_get_image          = void* (*) (void* assembly);
  using fn_il2cpp_image_get_name              = const char* (*) (void* image);
  using fn_il2cpp_class_from_name             = void* (*) (void* image, const char* namespaze, const char* name);
  using fn_il2cpp_class_get_static_field_data = void* (*) (void* klass);
  using fn_il2cpp_class_get_method_from_name  = void* (*) (void* klass, const char* name, int argsCount);
  using fn_il2cpp_field_static_get_value      = void (*)(void* field, void* value);
  using fn_il2cpp_runtime_object_init         = void (*)(void* obj);
  using fn_il2cpp_object_new                  = void* (*) (void* klass);
  using fn_il2cpp_string_new                  = void* (*) (const char* str);
  using fn_il2cpp_class_get_fields            = void* (*) (void* klass, void** iter);
  using fn_il2cpp_field_get_name              = const char* (*) (void* field);
  using fn_il2cpp_field_get_offset            = size_t (*)(void* field);
  using fn_il2cpp_type_get_object             = void* (*) (void* type);
  using fn_il2cpp_class_get_type              = void* (*) (void* klass);
  using fn_il2cpp_object_get_class            = void* (*) (void* obj);

  // Globals
  HMODULE   hGameAssembly       = nullptr;
  uintptr_t gameAssemblyBase    = 0;
  void*     assemblyCSharpImage = nullptr;

  // Exported function pointers
  fn_il2cpp_domain_get                  ptr_domain_get                  = nullptr;
  fn_il2cpp_domain_get_assemblies       ptr_domain_get_assemblies       = nullptr;
  fn_il2cpp_assembly_get_image          ptr_assembly_get_image          = nullptr;
  fn_il2cpp_image_get_name              ptr_image_get_name              = nullptr;
  fn_il2cpp_class_from_name             ptr_class_from_name             = nullptr;
  fn_il2cpp_class_get_static_field_data ptr_class_get_static_field_data = nullptr;
  fn_il2cpp_class_get_method_from_name  ptr_class_get_method_from_name  = nullptr;
  fn_il2cpp_object_new                  ptr_object_new                  = nullptr;
  fn_il2cpp_string_new                  ptr_string_new                  = nullptr;
  fn_il2cpp_class_get_fields            ptr_class_get_fields            = nullptr;
  fn_il2cpp_field_get_name              ptr_field_get_name              = nullptr;
  fn_il2cpp_field_get_offset            ptr_field_get_offset            = nullptr;
  fn_il2cpp_class_get_type              ptr_class_get_type              = nullptr;
  fn_il2cpp_type_get_object             ptr_type_get_object             = nullptr;
  fn_il2cpp_object_get_class            ptr_object_get_class            = nullptr;

  bool Init()
  {
    hGameAssembly = GetModuleHandleA("GameAssembly.dll");
    if (!hGameAssembly)
      return false;

    gameAssemblyBase = reinterpret_cast<uintptr_t>(hGameAssembly);

#define RESOLVE(name) \
  ptr_##name = reinterpret_cast<fn_il2cpp_##name>(GetProcAddress(hGameAssembly, "il2cpp_" #name)); \
  if (!ptr_##name) \
    return false;

    RESOLVE(domain_get);
    RESOLVE(domain_get_assemblies);
    RESOLVE(assembly_get_image);
    RESOLVE(image_get_name);
    RESOLVE(class_from_name);
    RESOLVE(class_get_static_field_data);
    RESOLVE(class_get_method_from_name);
    RESOLVE(object_new);
    RESOLVE(string_new);
    RESOLVE(class_get_fields);
    RESOLVE(field_get_name);
    RESOLVE(field_get_offset);
    RESOLVE(object_get_class);

    ptr_class_get_type =
      reinterpret_cast<fn_il2cpp_class_get_type>(GetProcAddress(hGameAssembly, "il2cpp_class_get_type"));
    ptr_type_get_object =
      reinterpret_cast<fn_il2cpp_type_get_object>(GetProcAddress(hGameAssembly, "il2cpp_type_get_object"));

#undef RESOLVE

    auto domain = domain_get();
    if (!domain)
      return false;

    size_t asmCount   = 0;
    auto   assemblies = domain_get_assemblies(domain, &asmCount);
    if (!assemblies || asmCount == 0)
      return false;

    for (size_t i = 0; i < asmCount; i++) {
      auto img = assembly_get_image(assemblies[i]);
      if (!img)
        continue;
      const char* name = image_get_name(img);
      if (name && strstr(name, "Assembly-CSharp")) {
        assemblyCSharpImage = img;
        break;
      }
    }

    return assemblyCSharpImage != nullptr;
  }

  void* GetClass(const char* name, const char* namespaze)
  {
    if (!assemblyCSharpImage)
      return nullptr;
    return IL2CPP::class_from_name(assemblyCSharpImage, namespaze, name);
  }

  void* GetStaticFieldData(void* klass)
  {
    if (!klass || !ptr_class_get_static_field_data)
      return nullptr;
    return ptr_class_get_static_field_data(klass);
  }

  void* domain_get() { return ptr_domain_get ? ptr_domain_get() : nullptr; }
  void** domain_get_assemblies(void* domain, size_t* size)
  {
    return ptr_domain_get_assemblies ? ptr_domain_get_assemblies(domain, size) : nullptr;
  }
  void* assembly_get_image(void* assembly)
  {
    return ptr_assembly_get_image ? ptr_assembly_get_image(assembly) : nullptr;
  }
  const char* image_get_name(void* image) { return ptr_image_get_name ? ptr_image_get_name(image) : nullptr; }
  void* class_from_name(void* image, const char* namespaze, const char* name)
  {
    return ptr_class_from_name ? ptr_class_from_name(image, namespaze, name) : nullptr;
  }
  void* class_get_method_from_name(void* klass, const char* name, int argsCount)
  {
    return ptr_class_get_method_from_name ? ptr_class_get_method_from_name(klass, name, argsCount) : nullptr;
  }
  void* class_get_fields(void* klass, void** iter)
  {
    return ptr_class_get_fields ? ptr_class_get_fields(klass, iter) : nullptr;
  }
  const char* field_get_name(void* field) { return ptr_field_get_name ? ptr_field_get_name(field) : nullptr; }
  size_t field_get_offset(void* field) { return ptr_field_get_offset ? ptr_field_get_offset(field) : 0; }
  size_t GetFieldOffset(void* klass, const char* name)
  {
    if (!klass || !name)
      return 0;
    void* iter = nullptr;
    while (void* field = class_get_fields(klass, &iter)) {
      const char* fieldName = field_get_name(field);
      if (fieldName && strcmp(fieldName, name) == 0) {
        return field_get_offset(field);
      }
    }
    return 0;
  }

  void* object_get_class(void* obj)
  {
    if (!ptr_object_get_class)
      return nullptr;
    return ptr_object_get_class(obj);
  }

  void* string_new(const char* str)
  {
    if (!ptr_string_new)
      return nullptr;
    return ptr_string_new(str);
  }

  void* GetSystemTypeForClass(void* klass)
  {
    if (!klass || !ptr_class_get_type || !ptr_type_get_object)
      return nullptr;
    void* type = ptr_class_get_type(klass);
    if (!type)
      return nullptr;
    return ptr_type_get_object(type);
  }

  void* FindObjectsOfType(void* type)
  {
    using fn_FindObjectsOfType                     = void* (*) (void*);
    static fn_FindObjectsOfType pFindObjectsOfType = nullptr;

    if (!pFindObjectsOfType) {
      /*pFindObjectsOfType = reinterpret_cast<fn_FindObjectsOfType>(IL2CPP_METHOD(Object, FindObjectsOfType));*/
      void* coreModuleImage = nullptr;
      if (auto domain = domain_get()) {
        size_t asmCount = 0;
        if (auto assemblies = domain_get_assemblies(domain, &asmCount)) {
          for (size_t i = 0; i < asmCount; i++) {
            auto img = assembly_get_image(assemblies[i]);
            if (img && image_get_name(img) && strstr(image_get_name(img), "UnityEngine.CoreModule")) {
              coreModuleImage = img;
              break;
            }
          }
        }
      }
      if (coreModuleImage) {
        if (void* objectClass = class_from_name(coreModuleImage, "UnityEngine", "Object")) {
          if (void* method = class_get_method_from_name(objectClass, "FindObjectsOfType", 1)) {
            pFindObjectsOfType = reinterpret_cast<fn_FindObjectsOfType>(*reinterpret_cast<void**>(method));
          }
        }
      }
    }

    if (!pFindObjectsOfType || !type)
      return nullptr;
    return pFindObjectsOfType(type);
  }

  Vector3 GetTransformPosition(void* transform)
  {
    if (!transform)
      return {0, 0, 0};

    using fn_GetPosition               = Vector3 (*)(void*);
    static fn_GetPosition pGetPosition = nullptr;
    if (!pGetPosition) {
      void* coreModuleImage = nullptr;
      auto  domain          = domain_get();
      if (domain) {
        size_t asmCount   = 0;
        auto   assemblies = domain_get_assemblies(domain, &asmCount);
        if (assemblies) {
          for (size_t i = 0; i < asmCount; i++) {
            auto img = assembly_get_image(assemblies[i]);
            if (img && image_get_name(img) && strstr(image_get_name(img), "UnityEngine.CoreModule")) {
              coreModuleImage = img;
              break;
            }
          }
        }
      }
      if (coreModuleImage) {
        void* transformClass = class_from_name(coreModuleImage, "UnityEngine", "Transform");
        if (transformClass) {
          void* method = class_get_method_from_name(transformClass, "get_position", 0);
          if (method) {
            pGetPosition = reinterpret_cast<fn_GetPosition>(*reinterpret_cast<void**>(method));
          }
        }
      }
    }

    if (pGetPosition) {
      return pGetPosition(transform);
    }
    return {0, 0, 0};
  }

  void SetTransformPosition(void* transform, float x, float y, float z)
  {
    if (!transform)
      return;

    using fn_SetPosition               = void (*)(void*, Vector3);

    static fn_SetPosition pSetPosition = nullptr;
    if (!pSetPosition) {
      void* coreModuleImage = nullptr;
      auto  domain          = domain_get();
      if (domain) {
        size_t asmCount   = 0;
        auto   assemblies = domain_get_assemblies(domain, &asmCount);
        if (assemblies) {
          for (size_t i = 0; i < asmCount; i++) {
            auto img = assembly_get_image(assemblies[i]);
            if (img && image_get_name(img) && strstr(image_get_name(img), "UnityEngine.CoreModule")) {
              coreModuleImage = img;
              break;
            }
          }
        }
      }
      if (coreModuleImage) {
        void* transformClass = class_from_name(coreModuleImage, "UnityEngine", "Transform");
        if (transformClass) {
          void* method = class_get_method_from_name(transformClass, "set_position", 1);
          if (method) {
            pSetPosition = reinterpret_cast<fn_SetPosition>(*reinterpret_cast<void**>(method));
          }
        }
      }
    }

    if (pSetPosition) {
      Vector3 pos = {x, y, z};
      pSetPosition(transform, pos);
    }
  }

  void LookAt(void* transform, void* targetTransform)
  {
    if (!transform || !targetTransform)
      return;

    using fn_LookAt          = void (*)(void*, void*);
    static fn_LookAt pLookAt = nullptr;
    if (!pLookAt) {
      void* coreModuleImage = nullptr;
      auto  domain          = domain_get();
      if (domain) {
        size_t asmCount   = 0;
        auto   assemblies = domain_get_assemblies(domain, &asmCount);
        if (assemblies) {
          for (size_t i = 0; i < asmCount; i++) {
            auto img = assembly_get_image(assemblies[i]);
            if (img && image_get_name(img) && strstr(image_get_name(img), "UnityEngine.CoreModule")) {
              coreModuleImage = img;
              break;
            }
          }
        }
      }
      if (coreModuleImage) {
        void* transformClass = class_from_name(coreModuleImage, "UnityEngine", "Transform");
        if (transformClass) {
          void* method = class_get_method_from_name(transformClass, "LookAt", 1);
          if (method) {
            pLookAt = reinterpret_cast<fn_LookAt>(*reinterpret_cast<void**>(method));
          }
        }
      }
    }

    if (pLookAt) {
      pLookAt(transform, targetTransform);
    }
  }

  std::vector<void*> GetPlayers()
  {
    static std::vector<void*> cachedPlayers;
    static uint64_t           lastTime = 0;
    uint64_t                  now      = GetTickCount64();

    if (now - lastTime > 1000 || cachedPlayers.empty()) {
      lastTime = now;
      cachedPlayers.clear();

      void* pmcClass = reinterpret_cast<void*>(Offsets::Classes::PlayerMoveC);
      void* pmcType  = GetSystemTypeForClass(pmcClass);
      if (pmcType) {
        void* arrayObj = FindObjectsOfType(pmcType);
        if (arrayObj) {
          size_t length   = GetArrayLength(arrayObj);
          void** elements = GetArrayElements<void*>(arrayObj);
          if (elements) {
            cachedPlayers.reserve(length);
            for (size_t i = 0; i < length; i++) {
              if (elements[i]) {
                cachedPlayers.push_back(elements[i]);
              }
            }
          }
        }
      }
    }

    return cachedPlayers;
  }

  uintptr_t GetMethodAddress(uintptr_t rva) { return gameAssemblyBase + rva; }

  namespace
  {
    // Parses "48 8B ?? 05" -> bytes + wildcard mask (std::nullopt == wildcard)
    std::vector<std::optional<uint8_t>> ParsePattern(const char* sig)
    {
      std::vector<std::optional<uint8_t>> out;
      for (const char* p = sig; *p;) {
        if (*p == ' ') {
          ++p;
          continue;
        }
        if (*p == '?') {
          out.emplace_back(std::nullopt);
          while (*p == '?')
            ++p;
          continue;
        }
        char* end = nullptr;
        out.emplace_back(static_cast<uint8_t>(strtoul(p, &end, 16)));
        if (end == p)
          break;
        p = end;
      }
      return out;
    }

    // Returns the unique match address, or 0 if none / more than one.
    uintptr_t ScanUnique(const std::vector<std::optional<uint8_t>>& pat)
    {
      if (pat.empty() || !pat[0] || !gameAssemblyBase)
        return 0;

      auto dos        = reinterpret_cast<const IMAGE_DOS_HEADER*>(gameAssemblyBase);
      auto nt         = reinterpret_cast<const IMAGE_NT_HEADERS*>(gameAssemblyBase + dos->e_lfanew);
      auto sec        = IMAGE_FIRST_SECTION(nt);

      uintptr_t found = 0;
      for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec) {
        if (!(sec->Characteristics & IMAGE_SCN_MEM_EXECUTE))
          continue;
        auto*        begin = reinterpret_cast<const uint8_t*>(gameAssemblyBase + sec->VirtualAddress);
        const size_t size  = sec->Misc.VirtualSize;
        if (size < pat.size())
          continue;

        const uint8_t* cur  = begin;
        const uint8_t* last = begin + size - pat.size();
        while (cur <= last) {
          cur = static_cast<const uint8_t*>(memchr(cur, *pat[0], static_cast<size_t>(last - cur) + 1));
          if (!cur)
            break;
          size_t j = 1;
          while (j < pat.size() && (!pat[j] || cur[j] == *pat[j]))
            ++j;
          if (j == pat.size()) {
            if (found)
              return 0;  // ambiguous
            found = reinterpret_cast<uintptr_t>(cur);
          }
          ++cur;
        }
      }
      return found;
    }
  }  // namespace

  uintptr_t ResolveMethod(const char* sig, uintptr_t fallbackRva, const char* debugName)
  {
    static std::mutex                                 cacheMutex;
    static std::unordered_map<const char*, uintptr_t> cache;

    const uintptr_t fallback = gameAssemblyBase + fallbackRva;
    if (!sig || !*sig)
      return fallback;

    std::lock_guard lock(cacheMutex);
    if (auto it = cache.find(sig); it != cache.end())
      return it->second;

    uintptr_t addr = ScanUnique(ParsePattern(sig));
    if (!addr) {
      printf(
        "[SIG] %s: not found/ambiguous, using RVA 0x%llX\n", debugName ? debugName : sig,
        static_cast<unsigned long long>(fallbackRva)
      );
      addr = fallback;
    }
    else if (addr != fallback) {
      printf(
        "[SIG] %s: moved RVA 0x%llX -> 0x%llX (update offsets.h)\n", debugName ? debugName : sig,
        static_cast<unsigned long long>(fallbackRva), static_cast<unsigned long long>(addr - gameAssemblyBase)
      );
    }
    cache.emplace(sig, addr);
    return addr;
  }

  bool ResolveClasses()
  {
    Offsets::Classes::WeaponManager           = reinterpret_cast<uintptr_t>(GetClass("WeaponManager"));
    Offsets::Classes::PlayerMoveC             = reinterpret_cast<uintptr_t>(GetClass("Player_move_c"));
    Offsets::Classes::WeaponSounds            = reinterpret_cast<uintptr_t>(GetClass("WeaponSounds"));
    Offsets::Classes::PlayerDamageable        = reinterpret_cast<uintptr_t>(GetClass("PlayerDamageable"));

    Offsets::Classes::CheatDetectedBanner     = reinterpret_cast<uintptr_t>(GetClass("CheatDetectedBanner"));
    Offsets::Classes::ClickerDetector         = reinterpret_cast<uintptr_t>(GetClass("ClickerDetector"));

    Offsets::Classes::SkinName                = reinterpret_cast<uintptr_t>(GetClass("SkinName"));
    Offsets::Classes::FirstPersonControlSharp = reinterpret_cast<uintptr_t>(GetClass("FirstPersonControlSharp"));

    Offsets::Classes::NetworkStartTableNGUIController =
      reinterpret_cast<uintptr_t>(GetClass("NetworkStartTableNGUIController"));

    return Offsets::Classes::WeaponManager != 0 && Offsets::Classes::PlayerMoveC != 0
        && Offsets::Classes::WeaponSounds != 0;
  }

  void* GetWeaponManagerInstance()
  {
    auto klass = reinterpret_cast<void*>(Offsets::Classes::WeaponManager);
    if (!klass)
      return nullptr;

    auto staticData = GetStaticFieldData(klass);
    if (!staticData)
      return nullptr;

    return SafeReadField<void*>(staticData, Offsets::WeaponManager::StaticInstance);
  }

  void* GetLocalPlayerMoveC()
  {
    auto wm = GetWeaponManagerInstance();
    if (wm) {
      void* myPMC = SafeReadField<void*>(wm, Offsets::WeaponManager::myPlayerMoveC);
      if (myPMC)
        return myPMC;
    }

    // Fallback for Battle Royale and Free Mode
    Offsets::PlayerMoveC::InitDynamicOffsets();
    Offsets::SkinName::InitDynamicOffsets();

    if (Offsets::PlayerMoveC::mySkinNameOffset > 0 && Offsets::SkinName::isMineOffset > 0) {
      auto players = GetPlayers();
      for (auto pmc : players) {
        if (!pmc)
          continue;
        void* skinName = SafeReadField<void*>(pmc, Offsets::PlayerMoveC::mySkinNameOffset);
        if (skinName) {
          bool isMine = SafeReadField<bool>(skinName, Offsets::SkinName::isMineOffset);
          if (isMine)
            return pmc;
        }
      }
    }

    return nullptr;
  }

  void* GetCurrentWeaponSounds()
  {
    auto pmc = GetLocalPlayerMoveC();
    if (!pmc)
      return nullptr;
    return ReadField<void*>(pmc, Offsets::PlayerMoveC::weaponSoundsRef);
  }

  void* GetLocalPlayerDamageable()
  {
    auto pmc = GetLocalPlayerMoveC();
    if (!pmc)
      return nullptr;
    return ReadField<void*>(pmc, Offsets::PlayerMoveC::playerDamageable);
  }
}  // namespace IL2CPP

namespace Offsets
{
  namespace PlayerMoveC
  {
    void InitDynamicOffsets()
    {
      if (dynamicOffsetsResolved)
        return;

      void* pmcClass = (void*) Offsets::Classes::PlayerMoveC;
      if (!pmcClass)
        return;

      mySkinNameOffset       = IL2CPP::ResolveFieldOffset(pmcClass, {"mySkinName"});
      dynamicOffsetsResolved = true;
    }
  }  // namespace PlayerMoveC
}  // namespace Offsets

namespace Offsets
{
  namespace SkinName
  {
    void InitDynamicOffsets()
    {
      if (dynamicOffsetsResolved)
        return;

      void* skinNameClass = (void*) Offsets::Classes::SkinName;
      if (!skinNameClass)
        return;

      isMineOffset             = IL2CPP::ResolveFieldOffset(skinNameClass, {"isMine"});
      firstPersonControlOffset = IL2CPP::ResolveFieldOffset(skinNameClass, {"firstPersonControl"});

      dynamicOffsetsResolved   = true;
    }
  }  // namespace SkinName
}  // namespace Offsets

namespace Offsets
{
  namespace FirstPersonControlSharp
  {
    void InitDynamicOffsets()
    {
      if (dynamicOffsetsResolved)
        return;

      void* fpcClass = (void*) Offsets::Classes::FirstPersonControlSharp;
      if (!fpcClass)
        return;

      velocityDownFallMultiplierOffset = IL2CPP::ResolveFieldOffset(fpcClass, {"velocityDownFallMultiplier"});

      dynamicOffsetsResolved           = true;
    }
  }  // namespace FirstPersonControlSharp
}  // namespace Offsets

namespace Offsets
{
  namespace WeaponSounds
  {
    void InitDynamicOffsets()
    {
      if (dynamicOffsetsResolved)
        return;

      void* wsClass = (void*) Offsets::Classes::WeaponSounds;
      if (!wsClass)
        return;

      isUnlimitedAmmoOffset =
        IL2CPP::ResolveFieldOffset(wsClass, {"isUnlimitedAmmo"}, Offsets::WeaponSounds::isUnlimitedAmmo);

      canAffectAlliesOffset  = IL2CPP::ResolveFieldOffset(wsClass, {"canAffectAllies"});

      dynamicOffsetsResolved = true;
    }
  }  // namespace WeaponSounds
}  // namespace Offsets

namespace Offsets
{
  namespace LiveWeapon
  {
    void InitDynamicOffsets(void* charWeaponClass)
    {
      if (dynamicOffsetsResolved || !charWeaponClass)
        return;

      liveAmmoOffset = IL2CPP::ResolveFieldOffset(
        charWeaponClass, {"ammo", "currentAmmoInBackpack"}, Offsets::LiveWeapon::ammoFallback
      );

      dynamicOffsetsResolved = true;
    }
  }  // namespace LiveWeapon
}  // namespace Offsets
