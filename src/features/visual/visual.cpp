#include "visual.h"
#include "../../utils/il2cpp.h"
#include "../../ui/menu.h"
#include <windows.h>
#include <imgui.h>
#include <cmath>
#include <vector>

namespace Visual
{
  std::vector<PlayerESPData> cachedPlayers;
  std::mutex                 espMutex;
  float                      g_screenW      = 0.0f;
  float                      g_screenH      = 0.0f;
  uint64_t                   lastVisualTick = 0;


  using fn_WorldToScreenPoint               = Vector3 (*)(void* camera, Vector3 worldPos);
  fn_WorldToScreenPoint pWorldToScreenPoint = nullptr;

  using fn_GetPixelWidth                    = int (*)(void* camera);
  fn_GetPixelWidth pGetPixelWidth           = nullptr;

  using fn_GetPixelHeight                   = int (*)(void* camera);
  fn_GetPixelHeight pGetPixelHeight         = nullptr;


  using fn_GetMainCamera                    = void* (*) ();
  fn_GetMainCamera pGetMainCamera           = nullptr;

  fn_Linecast pLinecast                     = nullptr;

  // TextMesh text reading for ESP names
  using fn_GetText    = void* (*) (void* textMesh);
  fn_GetText pGetText = nullptr;

  // Helper: extract C-string from IL2CPP string object
  // IL2CPP string layout: [object header 0x10] [int32 length @ 0x10] [char16_t chars[] @ 0x14]
  static bool ReadIL2CPPString(void* il2cppStr, char* outBuf, size_t outBufSize)
  {
    if (!il2cppStr || outBufSize == 0)
      return false;

    int strLen =
      *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(il2cppStr) + Offsets::IL2CPPStructs::stringLengthOffset);
    if (strLen <= 0 || strLen > 256) {
      outBuf[0] = '\0';
      return false;
    }

    auto chars = reinterpret_cast<const wchar_t*>(
      reinterpret_cast<uintptr_t>(il2cppStr) + Offsets::IL2CPPStructs::stringCharsOffset
    );

    // Convert UTF-16 to UTF-8 using WideCharToMultiByte
    int bytesWritten =
      WideCharToMultiByte(CP_UTF8, 0, chars, strLen, outBuf, static_cast<int>(outBufSize - 1), nullptr, nullptr);

    if (bytesWritten > 0) {
      outBuf[bytesWritten] = '\0';
      return true;
    }

    outBuf[0] = '\0';
    return false;
  }

  bool ResolveUnityMethods()
  {
    auto hUnity = GetModuleHandleA("UnityPlayer.dll");
    if (!hUnity)
      return false;

    auto domain = IL2CPP::domain_get();
    if (!domain)
      return false;

    size_t asmCount       = 0;
    auto   assemblies     = IL2CPP::domain_get_assemblies(domain, &asmCount);

    void* coreModuleImage = nullptr;
    for (size_t i = 0; i < asmCount; i++) {
      auto img = IL2CPP::assembly_get_image(assemblies[i]);
      if (!img)
        continue;
      const char* name = IL2CPP::image_get_name(img);
      if (name && strstr(name, "UnityEngine.CoreModule")) {
        coreModuleImage = img;
        break;
      }
    }

    if (!coreModuleImage)
      return false;

    auto cameraClass = IL2CPP::class_from_name(coreModuleImage, "UnityEngine", "Camera");
    if (!cameraClass)
      return false;

    auto getMainMethod = IL2CPP::class_get_method_from_name(cameraClass, "get_main", 0);
    if (getMainMethod) {
      pGetMainCamera = *reinterpret_cast<fn_GetMainCamera*>(getMainMethod);
    }

    auto w2sMethod = IL2CPP::class_get_method_from_name(cameraClass, "WorldToScreenPoint", 1);
    if (w2sMethod) {
      pWorldToScreenPoint = *reinterpret_cast<fn_WorldToScreenPoint*>(w2sMethod);
    }

    auto getWidthMethod = IL2CPP::class_get_method_from_name(cameraClass, "get_pixelWidth", 0);
    if (getWidthMethod) {
      pGetPixelWidth = *reinterpret_cast<fn_GetPixelWidth*>(getWidthMethod);
    }

    auto getHeightMethod = IL2CPP::class_get_method_from_name(cameraClass, "get_pixelHeight", 0);
    if (getHeightMethod) {
      pGetPixelHeight = *reinterpret_cast<fn_GetPixelHeight*>(getHeightMethod);
    }

    // Resolve Physics::Linecast from PhysicsModule
    void* physicsModuleImage = nullptr;
    for (size_t i = 0; i < asmCount; i++) {
      auto img = IL2CPP::assembly_get_image(assemblies[i]);
      if (!img)
        continue;
      const char* name = IL2CPP::image_get_name(img);
      if (name && strstr(name, "UnityEngine.PhysicsModule")) {
        physicsModuleImage = img;
        break;
      }
    }
    if (physicsModuleImage) {
      auto physicsClass = IL2CPP::class_from_name(physicsModuleImage, "UnityEngine", "Physics");
      if (physicsClass) {
        auto linecastMethod = IL2CPP::class_get_method_from_name(physicsClass, "Linecast", 3);
        if (linecastMethod) {
          pLinecast = *reinterpret_cast<fn_Linecast*>(linecastMethod);
        }
      }
    }

    // Resolve TextMesh::get_text from TextRenderingModule
    void* textRenderingImage = nullptr;
    for (size_t i = 0; i < asmCount; i++) {
      auto img = IL2CPP::assembly_get_image(assemblies[i]);
      if (!img)
        continue;
      const char* name = IL2CPP::image_get_name(img);
      if (name && strstr(name, "UnityEngine.TextRenderingModule")) {
        textRenderingImage = img;
        break;
      }
    }

    if (textRenderingImage) {
      auto textMeshClass = IL2CPP::class_from_name(textRenderingImage, "UnityEngine", "TextMesh");
      if (textMeshClass) {
        auto getTextMethod = IL2CPP::class_get_method_from_name(textMeshClass, "get_text", 0);
        if (getTextMethod) {
          pGetText = *reinterpret_cast<fn_GetText*>(getTextMethod);
        }
      }
    }

    return pGetMainCamera != nullptr;
  }

  bool WorldToScreen(Vector3 worldPos, Vector2& screenOut, void* camera, float screenW, float screenH)
  {
    if (!pWorldToScreenPoint || !camera)
      return false;

    Vector3 screenPos = pWorldToScreenPoint(camera, worldPos);

    if (screenPos.z < 0.1f)
      return false;

    float unityW = screenW;
    float unityH = screenH;

    if (pGetPixelWidth && pGetPixelHeight) {
      int camW = pGetPixelWidth(camera);
      int camH = pGetPixelHeight(camera);
      if (camW > 0 && camH > 0) {
        unityW = static_cast<float>(camW);
        unityH = static_cast<float>(camH);
      }
    }

    screenOut.x = screenPos.x * (screenW / unityW);
    screenOut.y = (unityH - screenPos.y) * (screenH / unityH);

    return (screenOut.x >= 0 && screenOut.x <= screenW && screenOut.y >= 0 && screenOut.y <= screenH);
  }

  void TickMainThread()
  {
    try {
      if (!Settings::bPlayerESPBoxes && !Settings::bPlayerESPNames && !Settings::bSkeletonESP) {
        std::lock_guard<std::mutex> lock(espMutex);
        cachedPlayers.clear();
        return;
      }

      if (!pGetMainCamera || !pWorldToScreenPoint) {
        std::lock_guard<std::mutex> lock(espMutex);
        cachedPlayers.clear();
        return;
      }

      void* camera = pGetMainCamera();
      if (!camera) {
        std::lock_guard<std::mutex> lock(espMutex);
        cachedPlayers.clear();
        return;
      }

      auto localPMC  = IL2CPP::GetLocalPlayerMoveC();

      lastVisualTick = GetTickCount64();
      std::vector<PlayerESPData> newCache;
      auto                       players = IL2CPP::GetPlayers();
      newCache.reserve(players.size());

      static void* pIsDeadMethod    = nullptr;
      static void* pIsEnemyToMethod = nullptr;
      if (!pIsDeadMethod)
        pIsDeadMethod = reinterpret_cast<void*>(IL2CPP::GetMethodAddress(Offsets::PlayerDamageable::IsDead_RVA));
      if (!pIsEnemyToMethod)
        pIsEnemyToMethod = reinterpret_cast<void*>(IL2CPP::GetMethodAddress(Offsets::PlayerDamageable::IsEnemyTo_RVA));

      for (auto pmc : players) {
        if (!pmc || pmc == localPMC)
          continue;

        auto playerTransform = IL2CPP::SafeReadField<void*>(pmc, Offsets::PlayerMoveC::myPlayerTransform);
        if (!playerTransform)
          continue;

        // Skip dead players
        auto damageable = IL2CPP::SafeReadField<void*>(pmc, Offsets::PlayerMoveC::playerDamageable);
        if (damageable) {
          using fn_IsDead = bool (*)(void*);
          auto isDead     = reinterpret_cast<fn_IsDead>(pIsDeadMethod);
          if (isDead && isDead(damageable))
            continue;
        }

        auto headTransform      = IL2CPP::SafeReadField<void*>(pmc, Offsets::PlayerMoveC::PlayerHeadTransform);

        auto    targetPosIL2CPP = IL2CPP::GetTransformPosition(playerTransform);
        Vector3 targetPos       = {targetPosIL2CPP.x, targetPosIL2CPP.y, targetPosIL2CPP.z};

        Vector3 footPos         = targetPos;
        footPos.y -= 1.0f;  // Guess feet position

        Vector3 headPos = targetPos;
        if (headTransform && !IsBadReadPtr(headTransform, sizeof(void*))) {
          auto headPosIL2CPP = IL2CPP::GetTransformPosition(headTransform);
          headPos            = {headPosIL2CPP.x, headPosIL2CPP.y, headPosIL2CPP.z};
          headPos.y += 0.3f;  // Offset to top of head
        }
        else {
          headPos.y += 1.0f;  // Guess height
        }

        Vector2 footScreen, headScreen;
        if (
          !WorldToScreen(footPos, footScreen, camera, g_screenW, g_screenH)
          || !WorldToScreen(headPos, headScreen, camera, g_screenW, g_screenH)
        ) {
          continue;
        }

        // Is enemy? IsEnemyTo takes Player_move_c* as parameter (not PlayerDamageable*)
        bool isEnemy = true;

        if (Settings::bTreatAllAsEnemies) {
          isEnemy = true;
        }
        else if (damageable && localPMC) {
          using fn_IsEnemyTo = bool (*)(void*, void*);
          auto isEnemyTo     = reinterpret_cast<fn_IsEnemyTo>(pIsEnemyToMethod);
          if (isEnemyTo) {
            isEnemy = isEnemyTo(damageable, localPMC);
          }
        }

        if (!isEnemy) {
          continue;  // Skip teammates
        }

        bool  isVisible     = false;
        void* visibleObjRef = IL2CPP::SafeReadField<void*>(pmc, Offsets::PlayerMoveC::visibleObjRef);
        if (visibleObjRef) {
          isVisible = true;
        }

        // Read player name from nickLabel TextMesh
        char playerName[64] = {0};
        if (Settings::bPlayerESPNames && pGetText) {
          auto nickLabel = IL2CPP::SafeReadField<void*>(pmc, Offsets::PlayerMoveC::nickLabel);
          if (nickLabel) {
            void* il2cppStr = pGetText(nickLabel);
            ReadIL2CPPString(il2cppStr, playerName, sizeof(playerName));
          }
        }

        PlayerESPData data;
        data.pmc           = pmc;
        data.transform     = playerTransform;
        data.headTransform = headTransform;
        data.isEnemy       = isEnemy;
        data.isVisible     = isVisible;
        data.screenPos     = {footScreen.x, footScreen.y};
        data.screenTop     = {headScreen.x, headScreen.y};
        memcpy(data.name, playerName, sizeof(data.name));
        newCache.push_back(data);
      }

      std::lock_guard<std::mutex> lock(espMutex);
      cachedPlayers = newCache;
    } catch (...) {
      // Catch exceptions to prevent crashes
      std::lock_guard<std::mutex> lock(espMutex);
      cachedPlayers.clear();
    }
  }

  void RenderOverlay(void* pDrawList)
  {
    if (!Settings::bPlayerESPBoxes && !Settings::bPlayerESPNames && !Settings::bSkeletonESP)
      return;

    if (GetTickCount64() - lastVisualTick > 1000) {
      std::lock_guard<std::mutex> lock(espMutex);
      cachedPlayers.clear();
      return;
    }

    void* camera = pGetMainCamera();
    if (!camera)
      return;

    std::lock_guard<std::mutex> lock(espMutex);
    for (auto& player : cachedPlayers) {
      if (player.screenPos.x == 0 && player.screenPos.y == 0)
        continue;  // Skip if invalid

      if (Settings::bPlayerESPBoxes) {
        DrawPlayerBox(pDrawList, player.screenPos, player.screenTop, player.isEnemy, g_screenW, g_screenH);
      }
      if (Settings::bPlayerESPNames) {
        DrawPlayerName(pDrawList, player.screenPos, player.screenTop, player.name);
      }
      if (Settings::bSkeletonESP && player.pmc) {
        DrawPlayerSkeleton(pDrawList, player.screenPos, player.screenTop, player.isEnemy, player.pmc);
      }
    }
  }

  void InitMenu()
  {
    Menu::AddMenuItem({"-- VISUAL --", Menu::ItemType::Header});
    Menu::AddMenuItem({"ESP Boxes", Menu::ItemType::Bool, &Settings::bPlayerESPBoxes});
    Menu::AddMenuItem({"ESP Names", Menu::ItemType::Bool, &Settings::bPlayerESPNames});
    Menu::AddMenuItem({"ESP Bones", Menu::ItemType::Bool, &Settings::bSkeletonESP});
    Menu::AddMenuItem({"ESP Team", Menu::ItemType::Bool, &Settings::bTreatAllAsEnemies});
  }
}  // namespace Visual
