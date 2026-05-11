#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <numeric>


struct GamepadStats {
    uint64_t last_timestamp = 0;
    std::vector<double> history;
    const size_t max_samples = 128;

    double avg_latency_ms = 0.0;
    double polling_rate_hz = 0.0;
    double peak_polling_rate_hz = 0.0;

    void update(uint64_t event_ns) {
        if (last_timestamp != 0 && event_ns > last_timestamp) {
            double delta_ms = static_cast<double>(event_ns - last_timestamp) / 1000000.0;

            if (delta_ms < 500.0 && delta_ms > 0.0) {
                history.push_back(delta_ms);
                if (history.size() > max_samples) {
                    history.erase(history.begin());
                }

                double sum = std::accumulate(history.begin(), history.end(), 0.0);
                avg_latency_ms = sum / history.size();

                if (avg_latency_ms > 0) {
                    polling_rate_hz = 1000.0 / avg_latency_ms;
                    if (polling_rate_hz > peak_polling_rate_hz) {
                        peak_polling_rate_hz = polling_rate_hz;
                    }
                }
            }
        }
        last_timestamp = event_ns;
    }

    void reset() {
        last_timestamp = 0;
        history.clear();
        avg_latency_ms = 0.0;
        polling_rate_hz = 0.0;
        peak_polling_rate_hz = 0.0;
    }
};

struct TouchpadFinger {
    bool down = false;
    float x = 0.0f;
    float y = 0.0f;
    float pressure = 0.0f;
};



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

    const GamepadStats& getStats() const { return m_stats; };

    bool isButtonPressed(SDL_GamepadButton button);
    float getAxis(SDL_GamepadAxis axis) {
        if (!m_gamepad) return 0.0f;
        // SDL3 returns from 0 to 32767 for triggers
        int16_t raw = SDL_GetGamepadAxis(m_gamepad, axis);
        return (float)raw / 32767.0f;
    }

    void passEvent(SDL_Event* event);
    void rumble(float weak_motor, float strong_motor, uint32_t duration);


    TouchpadFinger getTouchpadFinger(int touchpadIndex, int fingerIndex) {
        TouchpadFinger data;
        if (m_gamepad) {
            SDL_GetGamepadTouchpadFinger(m_gamepad, touchpadIndex, fingerIndex,
                                         &data.down, &data.x, &data.y, &data.pressure);
        }
        return data;
    }

    int getNumTouchpadFingers(int touchpadIndex) {
        return m_gamepad ? SDL_GetNumGamepadTouchpadFingers(m_gamepad, touchpadIndex) : 0;
    }

private:
    SDL_Gamepad* m_gamepad{ nullptr };
    GamepadStats m_stats;
};
