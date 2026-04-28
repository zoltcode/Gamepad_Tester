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

struct ButtonElement {
    SDL_GamepadButton button;
    float x;            // Offset from anchor.x
    float y;            // Offset from anchor.y
    float radius = 15.0f; // Default size for ABXY
};

static const ButtonElement CONTROLLER_MAP[] = {
    { SDL_GAMEPAD_BUTTON_SOUTH, 460.0f, 260.0f }, // A
    { SDL_GAMEPAD_BUTTON_EAST,  500.0f, 220.0f }, // B
    { SDL_GAMEPAD_BUTTON_WEST,  420.0f, 220.0f }, // X
    { SDL_GAMEPAD_BUTTON_NORTH, 460.0f, 180.0f }, // Y

    // Smaller buttons (View/Menu)
    { SDL_GAMEPAD_BUTTON_BACK,  263.0f, 219.0f, 10.0f },
    { SDL_GAMEPAD_BUTTON_START, 351.0f, 219.0f, 10.0f },

    // Stick clicks (L3/R3)
    { SDL_GAMEPAD_BUTTON_LEFT_STICK, 155.0f, 220.0f, 30.0f },
    { SDL_GAMEPAD_BUTTON_RIGHT_STICK, 380.0f, 310.0f, 30.0f }
};


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
