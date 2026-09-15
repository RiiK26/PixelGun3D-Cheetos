#pragma once

#include <d3d11.h>
#include <dxgi.h>

#include <string>

namespace Menu
{
  enum class ItemType
  {
    Header,
    Bool,
    Float,
    Int,
    Action
  };

  struct MenuItem
  {
    std::string name;
    ItemType    type;
    bool*       bValue;
    float*      fValue;
    int*        iValue;
    float       fMin;
    float       fMax;
    float       fStep;
    int         iMin;
    int         iMax;
    int         iStep;
    void (*action)();
    bool isExpanded;

    // Default Constructor
    MenuItem() = default;

    // Header Constructor
    MenuItem(std::string name_, ItemType type_, bool isExpanded_ = true) :
        name(name_),
        type(type_),
        bValue(nullptr),
        fValue(nullptr),
        iValue(nullptr),
        fMin(0),
        fMax(0),
        fStep(0),
        iMin(0),
        iMax(0),
        iStep(0),
        action(nullptr),
        isExpanded(isExpanded_)
    {
    }

    // Bool Constructor
    MenuItem(std::string name_, ItemType type_, bool* bValue_) :
        name(name_),
        type(type_),
        bValue(bValue_),
        fValue(nullptr),
        iValue(nullptr),
        fMin(0),
        fMax(0),
        fStep(0),
        iMin(0),
        iMax(0),
        iStep(0),
        action(nullptr),
        isExpanded(true)
    {
    }

    // Float Constructor
    MenuItem(std::string name_, ItemType type_, bool* bValue_, float* fValue_, float fMin_, float fMax_, float fStep_) :
        name(name_),
        type(type_),
        bValue(bValue_),
        fValue(fValue_),
        iValue(nullptr),
        fMin(fMin_),
        fMax(fMax_),
        fStep(fStep_),
        iMin(0),
        iMax(0),
        iStep(0),
        action(nullptr),
        isExpanded(true)
    {
    }

    // Int Constructor
    MenuItem(
      std::string name_,
      ItemType    type_,
      bool*       bValue_,
      float*      fValue_,
      int*        iValue_,
      float       fMin_,
      float       fMax_,
      float       fStep_,
      int         iMin_,
      int         iMax_,
      int         iStep_
    ) :
        name(name_),
        type(type_),
        bValue(bValue_),
        fValue(fValue_),
        iValue(iValue_),
        fMin(fMin_),
        fMax(fMax_),
        fStep(fStep_),
        iMin(iMin_),
        iMax(iMax_),
        iStep(iStep_),
        action(nullptr),
        isExpanded(true)
    {
    }

    // Action Constructor
    MenuItem(
      std::string name_,
      ItemType    type_,
      bool*       bValue_,
      float*      fValue_,
      int*        iValue_,
      float       fMin_,
      float       fMax_,
      float       fStep_,
      int         iMin_,
      int         iMax_,
      int         iStep_,
      void (*action_)()
    ) :
        name(name_),
        type(type_),
        bValue(bValue_),
        fValue(fValue_),
        iValue(iValue_),
        fMin(fMin_),
        fMax(fMax_),
        fStep(fStep_),
        iMin(iMin_),
        iMax(iMax_),
        iStep(iStep_),
        action(action_),
        isExpanded(true)
    {
    }

    // Old Action Constructor (from before adding Int)
    MenuItem(
      std::string name_,
      ItemType    type_,
      bool*       bValue_,
      float*      fValue_,
      float       fMin_,
      float       fMax_,
      float       fStep_,
      void (*action_)()
    ) :
        name(name_),
        type(type_),
        bValue(bValue_),
        fValue(fValue_),
        iValue(nullptr),
        fMin(fMin_),
        fMax(fMax_),
        fStep(fStep_),
        iMin(0),
        iMax(0),
        iStep(0),
        action(action_),
        isExpanded(true)
    {
    }
  };

  // D3D11 state
  extern ID3D11Device*           pDevice;
  extern ID3D11DeviceContext*    pContext;
  extern ID3D11RenderTargetView* pRenderTarget;
  extern IDXGISwapChain*         pSwapChain;

  // Window
  extern HWND    hGameWindow;
  extern WNDPROC oWndProc;

  // State
  extern bool initialized;

  void AddMenuItem(const MenuItem& item);
  bool InitD3D11(IDXGISwapChain* sc);
  void Render();
  void Shutdown();
}  // namespace Menu
