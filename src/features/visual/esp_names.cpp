#include "visual.h"
#include <imgui.h>

namespace Visual
{
  void DrawPlayerName(void* drawList, Vector2 footScreen, Vector2 screenTop, const char* name)
  {
    if (!name || !name[0])
      return;

    float height = footScreen.y - screenTop.y;
    if (height < 5.0f)
      return;

    float centerX  = (footScreen.x + screenTop.x) * 0.5f;
    float top      = screenTop.y;

    ImDrawList* dl = reinterpret_cast<ImDrawList*>(drawList);
    dl->AddText(ImVec2(centerX - 20.0f, top - 14.0f), 0xFFFFFFFF, name);
  }
}  // namespace Visual
