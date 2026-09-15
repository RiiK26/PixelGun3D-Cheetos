#include "menu.h"
#include "../utils/config.h"
#include "../features/visual/visual.h"
#include "../features/playermod/playermod.h"
#include "../features/weaponmod/weaponmod.h"
#include "../features/currency/currency.h"
#include "../utils/hooks.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <vector>
#include <string>
#include <algorithm>

namespace Menu
{
  ID3D11Device*           pDevice       = nullptr;
  ID3D11DeviceContext*    pContext      = nullptr;
  ID3D11RenderTargetView* pRenderTarget = nullptr;
  IDXGISwapChain*         pSwapChain    = nullptr;

  HWND    hGameWindow                   = nullptr;
  WNDPROC oWndProc                      = nullptr;

  bool initialized                      = false;

  namespace Settings
  {
    inline bool bMenuOpen = true;
  }

  std::vector<MenuItem> items;
  int                   selectedIndex = 0;
  int                   scrollOffset  = 0;

  void AddMenuItem(const MenuItem& item) { items.push_back(item); }

  void SetupItems()
  {
    items.clear();

    Visual::InitMenu();
    WeaponMod::InitMenu();
    PlayerMod::InitMenu();

    CurrencyMod::InitMenu();
    Hooks::SetupMenu();
  }

  std::vector<int> GetVisibleIndices()
  {
    std::vector<int> visible;
    bool             currentHeaderExpanded = true;
    for (int i = 0; i < items.size(); ++i) {
      if (items[i].type == ItemType::Header) {
        currentHeaderExpanded = items[i].isExpanded;
        visible.push_back(i);
      }
      else if (currentHeaderExpanded) {
        visible.push_back(i);
      }
    }
    return visible;
  }

  void MoveSelection(int dir)
  {
    if (items.empty())
      return;

    std::vector<int> visible = GetVisibleIndices();
    if (visible.empty())
      return;

    // Find current index in visible list
    auto it           = std::find(visible.begin(), visible.end(), selectedIndex);
    int  visibleIndex = 0;
    if (it != visible.end()) {
      visibleIndex = std::distance(visible.begin(), it);
    }

    visibleIndex += dir;
    if (visibleIndex < 0)
      visibleIndex = visible.size() - 1;
    if (visibleIndex >= visible.size())
      visibleIndex = 0;

    selectedIndex = visible[visibleIndex];
  }

  void Interact(int dir)
  {
    if (items.empty() || selectedIndex < 0 || selectedIndex >= items.size())
      return;
    auto& item = items[selectedIndex];

    if (item.type == ItemType::Header) {
      if (dir != 0)
        item.isExpanded = !item.isExpanded;
    }
    else if (item.type == ItemType::Bool && item.bValue) {
      if (dir < 0) {
        *item.bValue = false;
      }
      else if (dir > 0 && dir != 2) {  // 1 is right, 2 is enter
        *item.bValue = true;
      }
      else if (dir == 2) {
        *item.bValue = !(*item.bValue);  // Toggle on enter
      }
    }
    else if (item.type == ItemType::Float && item.fValue) {
      if (dir > 0) {
        *item.fValue += item.fStep;
        if (*item.fValue > item.fMax)
          *item.fValue = item.fMax;
      }
      else if (dir < 0) {
        *item.fValue -= item.fStep;
        if (*item.fValue < item.fMin)
          *item.fValue = item.fMin;
      }
    }
    else if (item.type == ItemType::Int && item.iValue) {
      if (dir > 0) {
        *item.iValue += item.iStep;
        if (*item.iValue > item.iMax)
          *item.iValue = item.iMax;
      }
      else if (dir < 0) {
        *item.iValue -= item.iStep;
        if (*item.iValue < item.iMin)
          *item.iValue = item.iMin;
      }
    }
    else if (item.type == ItemType::Action && item.action) {
      if (dir != 0)
        item.action();
    }
  }

  LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    if (msg == WM_KEYDOWN) {
      if (wParam == VK_INSERT) {
        Settings::bMenuOpen = !Settings::bMenuOpen;
        return 0;
      }

      if (Settings::bMenuOpen) {
        if (wParam == VK_UP) {
          MoveSelection(-1);
          return 0;
        }
        if (wParam == VK_DOWN) {
          MoveSelection(1);
          return 0;
        }
        if (wParam == VK_LEFT) {
          Interact(-1);
          return 0;
        }
        if (wParam == VK_RIGHT) {
          Interact(1);
          return 0;
        }
        if (wParam == VK_RETURN) {
          Interact(2);
          return 0;
        }
      }
    }

    return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
  }

  bool InitD3D11(IDXGISwapChain* sc)
  {
    pSwapChain = sc;

    if (FAILED(sc->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pDevice))))
      return false;

    pDevice->GetImmediateContext(&pContext);

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(sc->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
      return false;

    pDevice->CreateRenderTargetView(backBuffer, nullptr, &pRenderTarget);
    backBuffer->Release();

    DXGI_SWAP_CHAIN_DESC desc;
    sc->GetDesc(&desc);
    hGameWindow = desc.OutputWindow;

    oWndProc =
      reinterpret_cast<WNDPROC>(SetWindowLongPtr(hGameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));

    ImGui::CreateContext();
    ImGuiIO& io    = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImFontConfig fontConfig;
    fontConfig.OversampleH        = 1;
    fontConfig.OversampleV        = 1;
    fontConfig.PixelSnapH         = true;

    static const ImWchar ranges[] = {
      0x0020, 0x00FF,  // Basic Latin + Latin Supplement
      0x0400, 0x052F,  // Cyrillic + Cyrillic Supplement
      0x2DE0, 0x2DFF,  // Cyrillic Extended-A
      0xA640, 0xA69F,  // Cyrillic Extended-B
      0x3000, 0x303F,  // CJK Symbols and Punctuation
      0x3040, 0x309F,  // Hiragana
      0x30A0, 0x30FF,  // Katakana
      0x31F0, 0x31FF,  // Katakana Phonetic Extensions
      0xFF00, 0xFFEF,  // Halfwidth and Fullwidth Forms
      0x4e00, 0x9FAF,  // CJK Ideographs
      0,
    };

    // Attempt to load MS Gothic for broad CJK + Cyrillic support
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msgothic.ttc", 16.0f, &fontConfig, ranges);

    ImGui_ImplWin32_Init(hGameWindow);
    ImGui_ImplDX11_Init(pDevice, pContext);

    ImGuiStyle& style               = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);

    SetupItems();
    initialized = true;
    return true;
  }

  void Render()
  {
    if (!Settings::bMenuOpen)
      return;

    auto drawList                   = ImGui::GetBackgroundDrawList();

    float screenH                   = ImGui::GetIO().DisplaySize.y;

    float startX                    = 50.0f;
    float startY                    = 50.0f;
    float rowHeight                 = 20.0f;
    float padding                   = 10.0f;

    std::vector<int> visibleIndices = GetVisibleIndices();
    if (visibleIndices.empty())
      return;

#ifndef PROJECT_VERSION
  #define PROJECT_VERSION "Unknown"
#endif

    std::string titleStr = std::string("Pixel Gun 3D Cheat v") + PROJECT_VERSION;

    // Calculate dynamic width
    float width  = 250.0f;  // base thinner width
    float titleW = ImGui::CalcTextSize(titleStr.c_str()).x + padding * 2;
    if (titleW > width)
      width = titleW;

    for (int idx : visibleIndices) {
      auto& item       = items[idx];
      float textW      = ImGui::CalcTextSize(item.name.c_str()).x;
      float itemTotalW = textW + 100.0f;  // space for values on the right
      if (itemTotalW > width) {
        width = itemTotalW;
      }
    }

    std::string footerText  = "[ARROWS] Navigate|[INSERT] Hide";
    float       footerTextW = ImGui::CalcTextSize(footerText.c_str()).x + padding * 2;
    if (footerTextW > width) {
      width = footerTextW;
    }

    // Calculate how many items can fit on screen safely
    float availableHeight = screenH - startY - (padding * 2) - 30.0f - 20.0f;  // 20.0f bottom margin
    int   maxVisibleItems = static_cast<int>(availableHeight / rowHeight);
    if (maxVisibleItems < 5)
      maxVisibleItems = 5;

    // Find current selected index in visible list
    auto it                   = std::find(visibleIndices.begin(), visibleIndices.end(), selectedIndex);
    int  selectedVisibleIndex = 0;
    if (it != visibleIndices.end()) {
      selectedVisibleIndex = std::distance(visibleIndices.begin(), it);
    }
    else {
      selectedIndex        = visibleIndices[0];
      selectedVisibleIndex = 0;
    }

    // Scroll logic
    if (selectedVisibleIndex < scrollOffset) {
      scrollOffset = selectedVisibleIndex;
    }
    else if (selectedVisibleIndex >= scrollOffset + maxVisibleItems) {
      scrollOffset = selectedVisibleIndex - maxVisibleItems + 1;
    }

    // Safety clamp
    if (scrollOffset > visibleIndices.size() - maxVisibleItems) {
      scrollOffset = std::max(0, (int) visibleIndices.size() - maxVisibleItems);
    }

    int   itemsToDraw = std::min(maxVisibleItems, (int) visibleIndices.size() - scrollOffset);
    float totalHeight = itemsToDraw * rowHeight + padding * 2 + 30.0f;

    // Draw background
    drawList->AddRectFilled(
      ImVec2(startX, startY), ImVec2(startX + width, startY + totalHeight), IM_COL32(20, 20, 25, 230)
    );
    drawList->AddRect(
      ImVec2(startX, startY), ImVec2(startX + width, startY + totalHeight), IM_COL32(100, 50, 200, 255), 0, 0, 2.0f
    );

    // Title
    drawList->AddText(ImVec2(startX + padding, startY + padding), IM_COL32(100, 200, 255, 255), titleStr.c_str());

    float currentY = startY + padding + 30.0f;

    for (int i = 0; i < itemsToDraw; ++i) {
      int   actualIndex = visibleIndices[scrollOffset + i];
      auto& item        = items[actualIndex];
      ImU32 textColor   = IM_COL32(200, 200, 200, 255);

      if (actualIndex == selectedIndex) {
        // Draw selection highlight
        drawList->AddRectFilled(
          ImVec2(startX, currentY), ImVec2(startX + width, currentY + rowHeight), IM_COL32(80, 40, 160, 180)
        );
        textColor = IM_COL32(255, 255, 255, 255);
      }

      if (item.type == ItemType::Header) {
        textColor              = IM_COL32(150, 150, 150, 255);
        std::string headerName = item.name + (item.isExpanded ? " [-]" : " [+]");
        drawList->AddText(ImVec2(startX + width / 2 - 40, currentY + 2), textColor, headerName.c_str());
      }
      else {
        drawList->AddText(ImVec2(startX + padding, currentY + 2), textColor, item.name.c_str());

        if (item.type == ItemType::Bool && item.bValue) {
          const char* valStr   = *item.bValue ? "[ ON ]" : "[ OFF ]";
          ImU32       valColor = *item.bValue ? IM_COL32(50, 255, 50, 255) : IM_COL32(255, 50, 50, 255);
          drawList->AddText(ImVec2(startX + width - 60, currentY + 2), valColor, valStr);
        }
        else if (item.type == ItemType::Float && item.fValue) {
          char buf[32];
          snprintf(buf, sizeof(buf), "< %.2f >", *item.fValue);
          drawList->AddText(ImVec2(startX + width - 80, currentY + 2), IM_COL32(255, 255, 100, 255), buf);
        }
        else if (item.type == ItemType::Int && item.iValue) {
          char buf[32];
          snprintf(buf, sizeof(buf), "< %d >", *item.iValue);
          drawList->AddText(ImVec2(startX + width - 80, currentY + 2), IM_COL32(255, 255, 100, 255), buf);
        }
        else if (item.type == ItemType::Action) {
          drawList->AddText(ImVec2(startX + width - 80, currentY + 2), IM_COL32(200, 200, 200, 255), "[ ENTER ]");
        }
      }

      currentY += rowHeight;
    }

    // Draw Footer
    float footerHeight = 25.0f;
    float footerStartY = startY + totalHeight;
    drawList->AddRectFilled(
      ImVec2(startX, footerStartY), ImVec2(startX + width, footerStartY + footerHeight), IM_COL32(30, 30, 40, 230)
    );
    drawList->AddRect(
      ImVec2(startX, footerStartY), ImVec2(startX + width, footerStartY + footerHeight), IM_COL32(100, 50, 200, 255), 0,
      0, 2.0f
    );

    footerText  = "[ARROWS] Navigate|[INSERT] Hide";
    footerTextW = ImGui::CalcTextSize(footerText.c_str()).x;
    drawList->AddText(
      ImVec2(startX + (width - footerTextW) / 2.0f, footerStartY + 5.0f), IM_COL32(150, 150, 150, 255),
      footerText.c_str()
    );
  }

  void Shutdown()
  {
    if (!initialized)
      return;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (oWndProc && hGameWindow)
      SetWindowLongPtr(hGameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));

    if (pRenderTarget)
      pRenderTarget->Release();
    if (pContext)
      pContext->Release();
    if (pDevice)
      pDevice->Release();

    initialized = false;
  }
}  // namespace Menu
