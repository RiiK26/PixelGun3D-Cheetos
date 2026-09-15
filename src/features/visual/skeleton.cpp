#include "visual.h"
#include "../../utils/il2cpp.h"
#include <imgui.h>

namespace Visual
{
  void DrawPlayerSkeleton(void* drawList, Vector2 footScreen, Vector2 screenTop, bool isEnemy, void* pmc)
  {
    ImDrawList*  dl    = reinterpret_cast<ImDrawList*>(drawList);
    unsigned int color = isEnemy ? 0xFF0000FF : 0xFF00FF00;

    // Try True Bone first
    bool trueBoneSuccess = false;
    if (pmc) {
      void* bodyRenderer = IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::playerBodyRenderer);
      if (bodyRenderer) {
        static void* getBonesMethod = nullptr;
        if (!getBonesMethod) {
          void* smrClass = IL2CPP::class_from_name(IL2CPP::assemblyCSharpImage, "UnityEngine", "SkinnedMeshRenderer");
          if (!smrClass) {
            // SkinnedMeshRenderer is in UnityEngine.CoreModule, let's find it properly
            // Actually it's in UnityEngine.CoreModule. We need to find the correct class.
          }
        }
        // Without proper method pointers, we will fall back to basic math.
      }
    }

    if (!trueBoneSuccess) {
      // Fallback to basic math
      float height = footScreen.y - screenTop.y;
      if (height < 5.0f)
        return;

      float neckY         = screenTop.y + height * 0.15f;
      float pelvisY       = screenTop.y + height * 0.55f;
      float shoulderWidth = height * 0.45f;

      // Spine
      dl->AddLine(ImVec2(screenTop.x, neckY), ImVec2(screenTop.x, pelvisY), color, 1.5f);
      // Shoulders
      dl->AddLine(
        ImVec2(screenTop.x - shoulderWidth / 2, neckY), ImVec2(screenTop.x + shoulderWidth / 2, neckY), color, 1.5f
      );
      // Arms
      dl->AddLine(
        ImVec2(screenTop.x - shoulderWidth / 2, neckY), ImVec2(screenTop.x - shoulderWidth / 2, pelvisY), color, 1.5f
      );
      dl->AddLine(
        ImVec2(screenTop.x + shoulderWidth / 2, neckY), ImVec2(screenTop.x + shoulderWidth / 2, pelvisY), color, 1.5f
      );
      // Legs
      dl->AddLine(ImVec2(screenTop.x, pelvisY), ImVec2(footScreen.x - shoulderWidth / 2, footScreen.y), color, 1.5f);
      dl->AddLine(ImVec2(screenTop.x, pelvisY), ImVec2(footScreen.x + shoulderWidth / 2, footScreen.y), color, 1.5f);
    }
  }
}  // namespace Visual
