#include <windows.h>
#include <tlhelp32.h>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>

// Console colors
#define COLOR_DEFAULT 7
#define COLOR_GREEN   10
#define COLOR_RED     12
#define COLOR_CYAN    11
#define COLOR_YELLOW  14
#define COLOR_PURPLE  13

void SetColor(int color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }

#ifndef PROJECT_VERSION
  #define PROJECT_VERSION "Unknown"
#endif

void PrintBanner()
{
  SetColor(COLOR_PURPLE);
  std::string title   = std::string("      PIXEL GUN 3D PC TRAINER v") + PROJECT_VERSION;
  int         padding = 46 - (int) title.length();
  std::string padStr(padding > 0 ? padding : 0, ' ');

  std::cout << "\n"
            << "    ╔══════════════════════════════════════════════╗\n"
            << "    ║" << title << padStr << "║\n"
            << "    ║                                              ║\n"
            << "    ║       DLL Injector                           ║\n"
            << "    ╚══════════════════════════════════════════════╝\n"
            << std::endl;
  SetColor(COLOR_DEFAULT);
}

// Convert wstring to lowercase for case-insensitive comparison
static std::wstring ToLower(const std::wstring& s)
{
  std::wstring result = s;
  std::transform(result.begin(), result.end(), result.begin(), ::towlower);
  return result;
}

// Known process names for Pixel Gun 3D
static const std::vector<std::wstring> GAME_PROCESS_NAMES = {
  L"Pixel Gun 3D.exe",
  L"PixelGun3D.exe",
  L"Pixel Gun 3D PC Edition.exe",
  L"pg3d.exe",
};

// Find process ID by trying multiple known names
// Also does a partial match as fallback: any exe containing "pixel" and "gun"
struct ProcessResult
{
  DWORD        pid = 0;
  std::wstring matchedName;
};

bool IsExactMatch(const std::wstring& exeName)
{
  for (const auto& name : GAME_PROCESS_NAMES) {
    if (_wcsicmp(exeName.c_str(), name.c_str()) == 0) {
      return true;
    }
  }
  return false;
}

bool IsPartialMatch(const std::wstring& exeName)
{
  std::wstring lower = ToLower(exeName);
  return lower.find(L"pixel") != std::wstring::npos && lower.find(L"gun") != std::wstring::npos
      && lower.find(L".exe") != std::wstring::npos;
}

ProcessResult FindGameProcess()
{
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE)
    return {};

  PROCESSENTRY32W pe;
  pe.dwSize = sizeof(pe);

  ProcessResult partialMatch;

  if (Process32FirstW(snapshot, &pe)) {
    do {
      if (IsExactMatch(pe.szExeFile)) {
        CloseHandle(snapshot);
        return {pe.th32ProcessID, pe.szExeFile};
      }

      if (partialMatch.pid == 0 && IsPartialMatch(pe.szExeFile)) {
        partialMatch = {pe.th32ProcessID, pe.szExeFile};
      }
    } while (Process32NextW(snapshot, &pe));
  }

  CloseHandle(snapshot);
  return partialMatch;
}

// Check if a module is loaded in the target process
bool IsModuleLoaded(DWORD pid, const wchar_t* moduleName)
{
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
  if (snapshot == INVALID_HANDLE_VALUE)
    return false;

  MODULEENTRY32W me;
  me.dwSize = sizeof(me);

  if (Module32FirstW(snapshot, &me)) {
    do {
      if (_wcsicmp(me.szModule, moduleName) == 0) {
        CloseHandle(snapshot);
        return true;
      }
    } while (Module32NextW(snapshot, &me));
  }

  CloseHandle(snapshot);
  return false;
}

// List all running processes (debug helper)
void ListProcesses()
{
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE)
    return;

  PROCESSENTRY32W pe;
  pe.dwSize = sizeof(pe);

  SetColor(COLOR_YELLOW);
  std::cout << "  [*] Listing all running processes:" << std::endl;
  SetColor(COLOR_DEFAULT);

  int count = 0;
  if (Process32FirstW(snapshot, &pe)) {
    do {
      std::wcout << L"      PID " << pe.th32ProcessID << L": " << pe.szExeFile << std::endl;
      count++;
    } while (Process32NextW(snapshot, &pe));
  }
  std::cout << "  [*] Total: " << count << " processes" << std::endl;
  CloseHandle(snapshot);
}

// Inject DLL via LoadLibraryW
bool InjectDLL(DWORD pid, const std::wstring& dllPath)
{
  // Open target process
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (!hProcess) {
    DWORD err = GetLastError();
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to open process (error " << err << ")." << std::endl;
    if (err == 5) {
      std::cout << "  [!] Access denied. Run as Administrator or in the same Wine prefix." << std::endl;
    }
    SetColor(COLOR_DEFAULT);
    return false;
  }

  SetColor(COLOR_CYAN);
  std::cout << "  [*] Process opened successfully." << std::endl;
  SetColor(COLOR_DEFAULT);

  // Allocate memory in target process for DLL path
  size_t pathSize  = (dllPath.length() + 1) * sizeof(wchar_t);
  void*  remoteMem = VirtualAllocEx(hProcess, nullptr, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!remoteMem) {
    DWORD err = GetLastError();
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to allocate memory in target (error " << err << ")." << std::endl;
    SetColor(COLOR_DEFAULT);
    CloseHandle(hProcess);
    return false;
  }

  // Write DLL path to target process
  if (!WriteProcessMemory(hProcess, remoteMem, dllPath.c_str(), pathSize, nullptr)) {
    DWORD err = GetLastError();
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to write DLL path to target (error " << err << ")." << std::endl;
    SetColor(COLOR_DEFAULT);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return false;
  }

  // Get LoadLibraryW address
  FARPROC loadLibrary = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
  if (!loadLibrary) {
    DWORD err = GetLastError();
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to resolve LoadLibraryW (error " << err << ")." << std::endl;
    SetColor(COLOR_DEFAULT);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return false;
  }

  // Create remote thread to call LoadLibraryW
  HANDLE hThread = CreateRemoteThread(
    hProcess, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibrary), remoteMem, 0, nullptr
  );

  if (!hThread) {
    DWORD err = GetLastError();
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to create remote thread (error " << err << ")." << std::endl;
    SetColor(COLOR_DEFAULT);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return false;
  }

  SetColor(COLOR_CYAN);
  std::cout << "  [*] Remote thread created, waiting for DLL load..." << std::endl;
  SetColor(COLOR_DEFAULT);

  // Wait for injection to complete
  WaitForSingleObject(hThread, 10000);

  // Check exit code
  DWORD exitCode = 0;
  GetExitCodeThread(hThread, &exitCode);

  // Cleanup
  CloseHandle(hThread);
  VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
  CloseHandle(hProcess);

  if (exitCode == 0) {
    SetColor(COLOR_RED);
    std::cout << "  [!] LoadLibrary returned 0 — DLL failed to load in target." << std::endl;
    std::cout << "  [!] Check that the DLL path is valid from the game's perspective." << std::endl;
    SetColor(COLOR_DEFAULT);
  }

  return exitCode != 0;
}

// Check if game process is still alive using the fuzzy matcher
bool IsGameAlive()
{
  auto result = FindGameProcess();
  return result.pid != 0;
}

int main(int argc, char* argv[])
{
  SetConsoleTitleW(L"PG3D Trainer Injector");
  PrintBanner();

  bool debugMode = false;
  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "--debug" || std::string(argv[i]) == "-d") {
      debugMode = true;
    }
  }

  // Find DLL path (same directory as injector)
  wchar_t exePath[MAX_PATH];
  GetModuleFileNameW(nullptr, exePath, MAX_PATH);
  std::filesystem::path dllPath = std::filesystem::path(exePath).parent_path() / L"trainer.dll";

  if (!std::filesystem::exists(dllPath)) {
    SetColor(COLOR_RED);
    std::cout << "  [!] trainer.dll not found!" << std::endl;
    std::cout << "  [!] Place trainer.dll next to the injector." << std::endl;
    SetColor(COLOR_DEFAULT);
    std::cout << "\n  Press any key to exit..." << std::endl;
    std::cin.get();
    return 1;
  }

  SetColor(COLOR_GREEN);
  std::cout << "  [+] Found trainer.dll" << std::endl;
  SetColor(COLOR_DEFAULT);

  // Show DLL path for debugging
  std::wcout << L"  [*] DLL path: " << dllPath.wstring() << std::endl;

  // Wait for game process
  SetColor(COLOR_YELLOW);
  std::cout << "  [*] Waiting for Pixel Gun 3D..." << std::endl;
  std::cout << "  [*] Scanning for: Pixel Gun 3D.exe, PixelGun3D.exe, pg3d.exe, ..." << std::endl;
  SetColor(COLOR_DEFAULT);

  // If debug mode, list all processes on first try
  if (debugMode) {
    ListProcesses();
  }

  ProcessResult gameProc;
  int           waitCount = 0;
  while (gameProc.pid == 0) {
    gameProc = FindGameProcess();
    if (gameProc.pid == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      waitCount++;
      // Every 10 seconds, print a status
      if (waitCount % 10 == 0) {
        SetColor(COLOR_YELLOW);
        std::cout << "  [*] Still waiting... (" << waitCount << "s elapsed)" << std::endl;
        if (waitCount == 30) {
          std::cout << "  [*] Tip: If the game is running, try --debug flag to list processes." << std::endl;
        }
        SetColor(COLOR_DEFAULT);
      }
    }
  }

  SetColor(COLOR_GREEN);
  std::cout << "  [+] Found game process (PID: " << gameProc.pid << ")" << std::endl;
  std::wcout << L"  [+] Matched process name: " << gameProc.matchedName << std::endl;
  SetColor(COLOR_DEFAULT);

  // Wait for GameAssembly.dll to load (IL2CPP runtime)
  SetColor(COLOR_YELLOW);
  std::cout << "  [*] Waiting for GameAssembly.dll..." << std::endl;
  SetColor(COLOR_DEFAULT);

  while (!IsModuleLoaded(gameProc.pid, L"GameAssembly.dll")) {
    if (!IsGameAlive()) {
      SetColor(COLOR_RED);
      return 0;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  SetColor(COLOR_GREEN);
  std::cout << "  [+] GameAssembly.dll loaded" << std::endl;
  SetColor(COLOR_DEFAULT);

  // Wait a bit for IL2CPP to fully initialize
  SetColor(COLOR_YELLOW);
  std::cout << "  [*] Waiting for IL2CPP initialization (3s)..." << std::endl;
  SetColor(COLOR_DEFAULT);
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Check if trainer is already injected
  if (IsModuleLoaded(gameProc.pid, L"trainer.dll")) {
    SetColor(COLOR_YELLOW);
    std::cout << "  [!] Trainer is already injected!" << std::endl;
    SetColor(COLOR_DEFAULT);
    return 0;
  }

  // Inject
  SetColor(COLOR_CYAN);
  std::cout << "  [*] Injecting trainer.dll..." << std::endl;
  SetColor(COLOR_DEFAULT);

  std::wstring fullDllPath = dllPath.wstring();
  if (InjectDLL(gameProc.pid, fullDllPath)) {
    SetColor(COLOR_GREEN);
    std::cout << "  [+] Injection successful!" << std::endl;
    std::cout << "  [+] Press INSERT in-game to toggle menu" << std::endl;
    std::cout << "  [+] Press END to unload trainer" << std::endl;
    SetColor(COLOR_DEFAULT);
  }
  else {
    SetColor(COLOR_RED);
    std::cout << "  [!] Injection failed!" << std::endl;
    std::cout << "  [!] Make sure you are running in the same Wine/Proton prefix as the game." << std::endl;
    std::cout << "  [!] On Linux, use: protontricks -c 'wine PG3D_Injector.exe' 2524890" << std::endl;
    SetColor(COLOR_DEFAULT);
  }

  // Wait a moment so the user can read the success message
  std::this_thread::sleep_for(std::chrono::seconds(2));
  return 0;
}
