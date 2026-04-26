#pragma once

#include <SDL3/SDL.h>
#include <iostream>


class Window {
public:
    Window(const char* title, int width, int height);
    ~Window();

    SDL_Window* getWindow() { return m_window; };
    SDL_Renderer* getRenderer() { return m_renderer; };

private:
    SDL_Window* m_window{ nullptr };
    SDL_Renderer* m_renderer{ nullptr };
};
