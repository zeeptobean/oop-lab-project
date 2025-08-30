#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "core/book_database.hpp"
#include "internal_utilities.hpp"
#include <map>
#include <string>

class TextureCache {
    public:
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *defaultTexture = nullptr;
    static TextureCache& get() {
        static TextureCache instance;
        return instance;
    }

    void init(SDL_Renderer* mainRenderer);

    void book_recache_all();

    SDL_Texture* book_get_texture(uint64_t bookId);

    private:
    std::map<uint64_t, SDL_Texture*> bookTextureMap;

    void book_destroy_all();

    TextureCache() {}
    ~TextureCache();
    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;
};