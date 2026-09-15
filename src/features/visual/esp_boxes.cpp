#include "visual.h"
#include <imgui.h>

namespace Visual
{
  void DrawPlayerBox(void* drawList, Vector2 footScreen, Vector2 screenTop, bool isEnemy, float screenW, float screenH)
  {
    float height = footScreen.y - screenTop.y;
    if (height < 5.0f)
      return;

    float width           = height * 0.85f;
    float centerX         = (footScreen.x + screenTop.x) * 0.5f;

    float left            = centerX - width * 0.5f;
    float right           = centerX + width * 0.5f;
    float top             = screenTop.y;
    float bottom          = footScreen.y;

    unsigned int boxColor = isEnemy ? 0xFF0000FF : 0xFF00FF00;

    ImDrawList* dl        = reinterpret_cast<ImDrawList*>(drawList);
    dl->AddRect(ImVec2(left, top), ImVec2(right, bottom), boxColor, 0.0f, 0, 1.5f);
  }
}  // namespace Visual
