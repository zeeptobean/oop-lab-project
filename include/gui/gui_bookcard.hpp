#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include <SDL3/SDL.h>
#include "gui_abstract.hpp"
#include "gui_bookview.hpp"
#include "imgui_custom_widget.hpp"
#include "../core/book.hpp"
#include "../app_context.hpp"
#include "../texture_cache.hpp"

class UIBookCard : public IUIAbstract {
    private:
    AppContext& appContext;
    SDL_Texture *texture = nullptr;
    Book *book;

    public:
    UIBookCard(AppContext& context) : appContext(context), book(nullptr) {}
    UIBookCard(AppContext& context, Book* tbook);
    void draw() override;

    virtual ~UIBookCard() {}
};