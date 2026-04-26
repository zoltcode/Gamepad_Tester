#pragma once

#include "Window.h"
#include "GamepadManager.h"
#include "imgui.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_sdlrenderer3.h"
#include "TextureLoader.h"
#include <iomanip>
#include <vector>

/*static const unsigned char controller_map[] = {
    #embed "assets/noun-xbox-controller-7424807.png"
};*/

class GUI {
public:
    GUI(Window& window, GamepadManager& gamepadManager);
    ~GUI();

    void newFrame();
    void render();
    void updateUI();

private:
    Window& m_window;
    GamepadManager& m_gamepad;

    float m_weakValue = 0.0f;
    float m_strongValue = 0.0f;

    SDL_Texture* m_texture{ nullptr };
};
