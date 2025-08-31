#include "texture_cache.hpp"

#include <fstream>
#include <iostream>

void TextureCache::init(SDL_Renderer* mainRenderer) {
    renderer = mainRenderer;
    SDL_Texture* defaultTexture = IMG_LoadTexture(renderer, "../data/book/default.png");
    book_recache_all();
}

void TextureCache::book_recache_all() {
    book_destroy_all();
    const std::vector<Book>& bookVec = BookDatabase::get().getAllBooks();
    for(auto& book : bookVec) {
        SDL_Texture* texture = IMG_LoadTexture(renderer, std::string("../" + book.imagePath).c_str());
        if(!texture) {
            bookTextureMap[book.internalId] = defaultTexture;
        }
        bookTextureMap[book.internalId] = texture;
    }
}

SDL_Texture* TextureCache::book_get_texture(uint64_t bookId) {
    auto it = bookTextureMap.find(bookId);
    if(it != bookTextureMap.end()) {
        return it->second;
    }
    return defaultTexture;
}

void TextureCache::book_destroy_all() {
    for(auto& [id, texture] : bookTextureMap) {
        if(texture != defaultTexture) {
            SDL_DestroyTexture(texture);
        }
    }
    bookTextureMap.clear();
}

TextureCache::~TextureCache() {
    book_destroy_all();
    SDL_DestroyTexture(defaultTexture);
}