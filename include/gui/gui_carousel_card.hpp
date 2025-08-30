#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_custom_widget.hpp"
#include "gui_abstract.hpp"
#include "gui_bookview.hpp"
#include "../core/book.hpp"
#include "texture_cache.hpp"
#include "app_context.hpp"
#include <string>

class UICarouselCard : public IUIAbstract {
private:
    AppContext& appContext;
    Book *book;
    SDL_Texture *imageTexture;
    int cardHeight = 400;

public:
    explicit UICarouselCard(AppContext& ctx, Book *tbook, int height)
        : appContext(ctx),
          book(tbook),
          imageTexture(TextureCache::get().book_get_texture(tbook->internalId)),
          cardHeight(height)
    {}

    void draw() override;
};