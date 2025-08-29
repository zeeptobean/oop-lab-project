#pragma once

#include <map>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "book.hpp"
#include "internal_utilities.hpp"

class TextureCache {
    public:
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *defaultTexture = nullptr;
    static TextureCache& get() {
        static TextureCache instance;
        return instance;
    }

    void init(SDL_Renderer* mainRenderer) {
        renderer = mainRenderer;
        SDL_Surface* surface = loadImage("../data/book/default.png");
        defaultTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        book_recache_all();
    }

    void book_recache_all() {
        book_destroy_all();
        const std::vector<Book>& bookVec = BookDatabase::get().getAllBooks();
        for(auto& book : bookVec) {
            SDL_Surface* surface = loadImage("../" + book.imagePath);
            if(!surface) {
                bookTextureMap[book.internalId] = defaultTexture;
                continue;
            }
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
            bookTextureMap[book.internalId] = texture;
        }
    }

    SDL_Texture* book_get_texture(uint64_t bookId) {
        auto it = bookTextureMap.find(bookId);
        if(it != bookTextureMap.end()) {
            return it->second;
        }
        return defaultTexture;
    }

    private:
    std::map<uint64_t, SDL_Texture*> bookTextureMap;

    void book_destroy_all() {
        for(auto& [id, texture] : bookTextureMap) {
            if(texture != defaultTexture) {
                SDL_DestroyTexture(texture);
            }
        }
        bookTextureMap.clear();
    }

    TextureCache() {}
    ~TextureCache() {
        book_destroy_all();
        SDL_DestroyTexture(defaultTexture);
    }
    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;
};