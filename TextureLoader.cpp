#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "TextureLoader.h"


SDL_Texture* TextureLoader::loadFromMemory(SDL_Renderer* renderer, const unsigned char* data, size_t size)
{
    int width, height, channels;
    unsigned char* pixels = stbi_load_from_memory(data, (int)size, &width, &height, &channels, 4);

    if (!pixels) return nullptr;

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                                             SDL_TEXTUREACCESS_STATIC, width, height);

    if (texture) {
        SDL_UpdateTexture(texture, NULL, pixels, width * 4);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    stbi_image_free(pixels);
    return texture;
}
