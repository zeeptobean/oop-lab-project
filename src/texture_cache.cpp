#include "texture_cache.hpp"

void TextureCache::init(SDL_Renderer* mainRenderer) {
    renderer = mainRenderer;
    SDL_Surface* surface = loadImage("../data/book/default.png");
    defaultTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    book_recache_all();
}

void TextureCache::book_recache_all() {
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