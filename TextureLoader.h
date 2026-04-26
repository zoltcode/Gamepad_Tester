#pragma once

#include <SDL3/SDL.h>


class TextureLoader {
public:
    static SDL_Texture* loadFromMemory(SDL_Renderer* renderer, const unsigned char* data, size_t size);
};
