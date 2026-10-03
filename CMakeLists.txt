cmake_minimum_required(VERSION 3.20)
file(READ "vcpkg.json" VCPKG_JSON_CONTENT)
string(JSON VCPKG_PROJECT_VERSION GET "${VCPKG_JSON_CONTENT}" "version")

project(PG3DTrainer VERSION ${VCPKG_PROJECT_VERSION} LANGUAGES CXX C RC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

add_compile_definitions("PROJECT_VERSION=\"${PROJECT_VERSION}\"")

find_package(minhook CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# ============================================
# Trainer DLL (injected into game process)
# ============================================
file(GLOB_RECURSE FEATURE_SRC "src/features/*.cpp")
add_library(trainer SHARED
    src/core/dllmain.cpp
    src/utils/config.cpp
    src/utils/il2cpp.cpp
    src/utils/hooks.cpp
    src/ui/menu.cpp
    ${FEATURE_SRC}
)
target_include_directories(trainer PRIVATE src)

set_target_properties(trainer PROPERTIES
    PREFIX ""
)

# Link vcpkg libraries
target_link_libraries(trainer PRIVATE
    minhook::minhook
    imgui::imgui
    nlohmann_json::nlohmann_json
    d3d11
    dxgi
    d3dcompiler
    dwmapi
)

# ============================================
# Standalone Injector (console app)
# ============================================
add_executable(injector
    src/core/injector.cpp
    resources/injector.rc
)
set_target_properties(injector PROPERTIES
    OUTPUT_NAME "PG3D_Injector"
)

if(MINGW)
    target_link_options(injector PRIVATE -static)
    target_link_options(trainer PRIVATE -static)
    target_link_options(injector PRIVATE $<$<CONFIG:Release>:-s>)
    target_link_options(trainer PRIVATE $<$<CONFIG:Release>:-s>)
endif()
