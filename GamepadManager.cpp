#include "GamepadManager.h"


void GamepadManager::passEvent(SDL_Event* event)
{
    if (event->type == SDL_EVENT_GAMEPAD_ADDED && !m_gamepad) {
        m_gamepad = SDL_OpenGamepad(event->gdevice.which);
    }

    if (event->type == SDL_EVENT_GAMEPAD_REMOVED) {
        if (m_gamepad) {
            SDL_CloseGamepad(m_gamepad);
            m_gamepad = nullptr;
        }
    }
}


void GamepadManager::rumble(float weak_motor, float strong_motor, uint32_t duration)
{
    uint16_t low_freq = (uint16_t)(strong_motor * 0xFFFF);
    uint16_t high_freq = (uint16_t)(weak_motor * 0xFFFF);
    SDL_RumbleGamepad(m_gamepad, low_freq, high_freq, duration);
}


bool GamepadManager::isButtonPressed(SDL_GamepadButton button)
{
    if (m_gamepad == nullptr) {
        return false;
    }

    return SDL_GetGamepadButton(m_gamepad, button);
}


