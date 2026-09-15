#pragma once
#include <vector>
#include <mutex>

namespace Visual
{
  namespace Settings
  {
    inline bool bPlayerESPBoxes    = false;
    inline bool bPlayerESPNames    = false;
    inline bool bSkeletonESP       = false;
    inline bool bTreatAllAsEnemies = false;
  }  // namespace Settings

  struct Vector3
  {
    float x, y, z;
  };
  struct Vector2
  {
    float x, y;
  };

  struct PlayerESPData
  {
    Vector2 screenPos;
    Vector2 screenTop;
    float   distance;
    bool    isEnemy;
    bool    isDead;
    bool    isVisible;
    char    name[64];
    void*   pmc;
    void*   transform;
    void*   headTransform;
  };

  extern std::vector<PlayerESPData> cachedPlayers;
  extern std::mutex                 espMutex;
  extern float                      g_screenW;
  extern float                      g_screenH;

  using fn_GetMainCamera = void* (*) ();
  extern fn_GetMainCamera pGetMainCamera;

  using fn_Linecast = bool (*)(Vector3 start, Vector3 end, int layerMask);
  extern fn_Linecast pLinecast;

  bool WorldToScreen(Vector3 worldPos, Vector2& screenOut, void* camera, float screenW, float screenH);

  void InitMenu();
  bool ResolveUnityMethods();
  void TickMainThread();
  void RenderOverlay(void* pDrawList);
  void DrawPlayerSkeleton(void* drawList, Vector2 footScreen, Vector2 screenTop, bool isEnemy, void* pmc);
  void DrawPlayerBox(void* drawList, Vector2 footScreen, Vector2 screenTop, bool isEnemy, float screenW, float screenH);
  void DrawPlayerName(void* drawList, Vector2 footScreen, Vector2 screenTop, const char* name);

  void* GetAimbotTarget();
}  // namespace Visual
