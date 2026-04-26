cmake_minimum_required(VERSION 3.10)
project(gamepad_tester
        VERSION 0.1.0
        LANGUAGES CXX)

# Standard C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDL3_DIR "IGNORE")
set(SDL3_FOUND FALSE)


include(FetchContent)

# 1. Fetch SDL3 directly from GitHub
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-3.4.4 # Or use a specific tag like 'release-3.2.0'
)

# 2. Make SDL3 targets available
# We disable features we don't need to speed up build
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)


# List of ImGui files (Core + SDL3/Renderer backends)
set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_sdlrenderer3.cpp
)

# Main executable
add_executable(gamepad_tester main.cpp ${IMGUI_SOURCES}
Window.cpp
GamepadManager.cpp
GUI.cpp
TextureLoader.cpp
)

# Linking SDL3
target_link_libraries(gamepad_tester SDL3::SDL3-static
X11 xkbcommon wayland-client wayland-egl wayland-cursor decor-0 m dl pthread
)

target_compile_definitions(gamepad_tester PRIVATE
    GAMEPAD_TESTER_VERSION="${PROJECT_VERSION}"
)

# Helpful: tell the compiler where to find ImGui headers in the same folder
target_include_directories(gamepad_tester PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/imgui
)
