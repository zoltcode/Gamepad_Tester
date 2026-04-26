#include "Window.h"


Window::Window(const char* title, int width, int height)
{
    m_window = SDL_CreateWindow(title, width, height, 0);
    if (!m_window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
    }

    m_renderer = SDL_CreateRenderer(m_window, NULL);
    if (!m_renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
    }
}


Window::~Window()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
}

