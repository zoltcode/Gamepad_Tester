#pragma once

#include <SDL3/SDL.h>


class GamepadManager {
public:
    GamepadManager() {};
    ~GamepadManager() = default;

    bool isConnected() { return m_gamepad != nullptr; };
    const char* getGamepadName()
    {
        if (this->isConnected()) {
            return SDL_GetGamepadName(m_gamepad);
        } else {
            return "";
        }
    };
    bool isButtonPressed(SDL_GamepadButton button);
    float getAxis(SDL_GamepadAxis axis) {
        if (!m_gamepad) return 0.0f;
        // SDL3 returns from 0 to 32767 for triggers
        int16_t raw = SDL_GetGamepadAxis(m_gamepad, axis);
        return (float)raw / 32767.0f;
    }

    void passEvent(SDL_Event* event);
    void rumble(float weak_motor, float strong_motor, uint32_t duration);

private:
    SDL_Gamepad* m_gamepad{ nullptr };
};
