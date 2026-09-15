#include "../utils/config.h"
#include "../utils/hooks.h"
#include "../utils/il2cpp.h"
#include "../utils/il2cpp.h"
#include "../ui/menu.h"

#include <windows.h>
#include <chrono>
#include <thread>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <MinHook.h>

// Feature modules
#include "../features/visual/visual.h"

// ---- D3D11 Present Hook via VTable ----
namespace DX11Hook
{
  using fn_Present                       = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
  inline fn_Present oPresent             = nullptr;
  using fn_ResizeBuffers                 = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
  inline fn_ResizeBuffers oResizeBuffers = nullptr;
  inline bool             hookInstalled  = false;

  HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
  {
    // First call: initialize ImGui
    if (!Menu::initialized) {
      Menu::InitD3D11(pSwapChain);
    }

    // Start ImGui frame
    if (Menu::initialized) {
      ImGui_ImplDX11_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();

      // Draw menu
      Menu::Render();

      // Draw ESP (on the background overlay)
      if (Visual::Settings::bPlayerESPBoxes || Visual::Settings::bPlayerESPNames || Visual::Settings::bSkeletonESP) {
        // Get screen dimensions
        DXGI_SWAP_CHAIN_DESC desc;
        pSwapChain->GetDesc(&desc);
        Visual::g_screenW = static_cast<float>(desc.BufferDesc.Width);
        Visual::g_screenH = static_cast<float>(desc.BufferDesc.Height);

        Visual::RenderOverlay(ImGui::GetBackgroundDrawList());
      }

      // Render ImGui
      ImGui::Render();
      Menu::pContext->OMSetRenderTargets(1, &Menu::pRenderTarget, nullptr);
      ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
  }

  HRESULT WINAPI hkResizeBuffers(
    IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags
  )
  {
    if (Menu::pRenderTarget) {
      Menu::pRenderTarget->Release();
      Menu::pRenderTarget = nullptr;
    }

    HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

    if (SUCCEEDED(hr) && Menu::pDevice) {
      ID3D11Texture2D* pBackBuffer = nullptr;
      if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer)))) {
        Menu::pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &Menu::pRenderTarget);
        pBackBuffer->Release();
      }
    }

    return hr;
  }

  // Find the D3D11 Present function by creating a dummy swap chain
  bool FindAndHookPresent()
  {
    // Create a temporary D3D11 device + swap chain to get the vtable
    WNDCLASSEXA wc   = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = DefWindowProcA;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = "PG3DDummy";
    RegisterClassExA(&wc);

    HWND hWnd = CreateWindowExA(
      0, wc.lpszClassName, "", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr
    );

    DXGI_SWAP_CHAIN_DESC scd          = {};
    scd.BufferCount                   = 1;
    scd.BufferDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width              = 100;
    scd.BufferDesc.Height             = 100;
    scd.BufferUsage                   = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow                  = hWnd;
    scd.SampleDesc.Count              = 1;
    scd.Windowed                      = TRUE;

    IDXGISwapChain*      tmpSwapChain = nullptr;
    ID3D11Device*        tmpDevice    = nullptr;
    ID3D11DeviceContext* tmpContext   = nullptr;

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT           hr = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &scd, &tmpSwapChain, &tmpDevice,
      &featureLevel, &tmpContext
    );

    if (FAILED(hr)) {
      DestroyWindow(hWnd);
      UnregisterClassA(wc.lpszClassName, wc.hInstance);
      return false;
    }

    // Get Present from vtable (index 8)
    void** vtable         = *reinterpret_cast<void***>(tmpSwapChain);
    void*  pPresent       = vtable[8];
    void*  pResizeBuffers = vtable[13];

    // Cleanup temp objects
    tmpSwapChain->Release();
    tmpDevice->Release();
    tmpContext->Release();
    DestroyWindow(hWnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);

    // Hook Present with MinHook
    if (MH_CreateHook(pPresent, reinterpret_cast<LPVOID>(&hkPresent), reinterpret_cast<void**>(&oPresent)) != MH_OK)
      return false;

    // Hook ResizeBuffers
    if (
      MH_CreateHook(
        pResizeBuffers, reinterpret_cast<LPVOID>(&hkResizeBuffers), reinterpret_cast<void**>(&oResizeBuffers)
      )
      != MH_OK
    )
      return false;

    if (MH_EnableHook(pPresent) != MH_OK)
      return false;

    if (MH_EnableHook(pResizeBuffers) != MH_OK)
      return false;

    hookInstalled = true;
    return true;
  }
}  // namespace DX11Hook

// ---- Main trainer thread ----
void TrainerThread(HMODULE hModule)
{
  // Wait for game to fully load
  while (!GetModuleHandleA("GameAssembly.dll"))
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Small delay to ensure IL2CPP is initialized
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // Initialize IL2CPP resolver
  if (!IL2CPP::Init()) {
    MessageBoxA(
      nullptr, "Failed to initialize IL2CPP resolver.\nGameAssembly.dll not found or exports missing.", "PG3D Trainer",
      MB_ICONERROR
    );
    FreeLibraryAndExitThread(hModule, 1);
    return;
  }

  // Resolve all game classes
  if (!IL2CPP::ResolveClasses()) {
    MessageBoxA(
      nullptr, "Failed to resolve game classes.\nGame version may be incompatible.", "PG3D Trainer", MB_ICONERROR
    );
    FreeLibraryAndExitThread(hModule, 1);
    return;
  }

  // Resolve Unity engine methods for ESP
  Visual::ResolveUnityMethods();

  // Set config path and load saved settings
  char dllPath[MAX_PATH];
  GetModuleFileNameA(hModule, dllPath, MAX_PATH);
  Config::SetConfigPath(dllPath);
  Config::Load();

  // Initialize MinHook and install game hooks (god mode, anti-cheat)
  if (!Hooks::Init()) {
    MessageBoxA(nullptr, "Failed to initialize hooks.\nMinHook error.", "PG3D Trainer", MB_ICONERROR);
    FreeLibraryAndExitThread(hModule, 1);
    return;
  }

  // Find and hook D3D11 Present (for ImGui rendering)
  if (!DX11Hook::FindAndHookPresent()) {
    MessageBoxA(nullptr, "Failed to hook D3D11 Present.\nRenderer hook failed.", "PG3D Trainer", MB_ICONERROR);
    Hooks::Shutdown();
    FreeLibraryAndExitThread(hModule, 1);
    return;
  }

  // Main loop — wait for unload key (END)
  while (true) {
    if (GetAsyncKeyState(VK_END) & 1)
      break;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Cleanup
  Config::Save();
  Menu::Shutdown();
  Hooks::Shutdown();

  // Give time for hooks to fully detach
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  FreeLibraryAndExitThread(hModule, 0);
}

// ---- DLL Entry Point ----
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID /*reserved*/)
{
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hModule);
    auto hThread =
      CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(TrainerThread), hModule, 0, nullptr);
    if (hThread)
      CloseHandle(hThread);
  }
  return TRUE;
}
