#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "../app_context.hpp"
#include "../texture_cache.hpp"
#include "gui_abstract.hpp"
#include "gui_bookview.hpp"
#include "imgui_custom_widget.hpp"

#include "../core/book.hpp"
#include "../core/borrowing_history.hpp"
#include "../core/borrowing_service.hpp"

struct SDL_Texture;

class UIBookView : public IUIAbstract {
private:
    SDL_Texture *imageTexture;
    Book book;
    AppContext& appContext;
    bool departmentRestrictionActivate = false;
    bool ageRestrictionActivate = false;
    bool borrowConfirmationTick = false;

    void drawImpl();

public:
    UIBookView(AppContext& ctx, const Book& tbook);
    void draw() override;
};