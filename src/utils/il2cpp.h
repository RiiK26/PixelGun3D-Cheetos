#pragma once
#include "offsets.h"
#include <windows.h>
#include <vector>

namespace IL2CPP
{
  // Module handles
  extern HMODULE   hGameAssembly;
  extern uintptr_t gameAssemblyBase;
  extern void*     assemblyCSharpImage;

  // Initialization
  bool Init();
  bool ResolveClasses();

  void* GetClass(const char* name, const char* namespaze = "");
  void* GetStaticFieldData(void* klass);
  uintptr_t GetMethodAddress(uintptr_t rva);

  void* domain_get();
  void** domain_get_assemblies(void* domain, size_t* size);
  void* assembly_get_image(void* assembly);
  const char* image_get_name(void* image);
  void* class_from_name(void* image, const char* namespaze, const char* name);
  void* class_get_method_from_name(void* klass, const char* name, int argsCount);
  void* class_get_fields(void* klass, void** iter);
  const char* field_get_name(void* field);
  size_t field_get_offset(void* field);
  size_t GetFieldOffset(void* klass, const char* name);
  void* object_get_class(void* obj);
  void* string_new(const char* str);

  template<typename T>
  inline T ReadField(void* obj, uintptr_t offset)
  {
    if (!obj)
      return T{};
    return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(obj) + offset);
  }

  template<typename T>
  inline T SafeReadField(void* obj, uintptr_t offset)
  {
    if (!obj)
      return T{};
    void* ptr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(obj) + offset);
    if (IsBadReadPtr(ptr, sizeof(T)))
      return T{};
    try {
      return *reinterpret_cast<T*>(ptr);
    } catch (...) {
      return T{};
    }
  }

  template<typename T>
  inline void WriteField(void* obj, uintptr_t offset, T value)
  {
    if (!obj)
      return;
    *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(obj) + offset) = value;
  }

  template<typename T>
  inline T ReadFieldIfValid(void* obj, uintptr_t offset, T defaultValue = T{})
  {
    if (offset > 0)
      return ReadField<T>(obj, offset);
    return defaultValue;
  }

  template<typename T>
  inline void WriteFieldIfValid(void* obj, uintptr_t offset, T value)
  {
    if (offset > 0)
      WriteField<T>(obj, offset, value);
  }

  inline size_t ResolveFieldOffset(void* klass, const std::vector<const char*>& names, size_t fallback = 0)
  {
    for (const char* name : names) {
      size_t offset = GetFieldOffset(klass, name);
      if (offset > 0)
        return offset;
    }
    return fallback;
  }

  // Array Access
  template<typename T>
  inline T* GetArrayElements(void* arrayObj)
  {
    if (!arrayObj)
      return nullptr;
    return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(arrayObj) + Offsets::IL2CPPStructs::arrayDataOffset);
  }

  inline size_t GetArrayLength(void* arrayObj)
  {
    if (!arrayObj)
      return 0;
    return *reinterpret_cast<size_t*>(
      reinterpret_cast<uintptr_t>(arrayObj) + Offsets::IL2CPPStructs::arrayLengthOffset
    );
  }

  // Unity Helpers
  struct Vector3
  {
    float x, y, z;
  };
  void* GetSystemTypeForClass(void* klass);
  void* FindObjectsOfType(void* type);
  Vector3 GetTransformPosition(void* transform);
  void SetTransformPosition(void* transform, float x, float y, float z);
  void LookAt(void* transform, void* targetTransform);

  // Player tracking
  std::vector<void*> GetPlayers();

  // Game specific helpers
  void* GetWeaponManagerInstance();
  void* GetLocalPlayerMoveC();
  void* GetCurrentWeaponSounds();
  void* GetLocalPlayerDamageable();
}  // namespace IL2CPP
