#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "../app_context.hpp"
#include "../texture_cache.hpp"
#include "../event_dispatcher.hpp"
#include "../event_concrete.hpp"

#include "gui_abstract.hpp"
#include "gui_bookview.hpp"
#include "imgui_custom_widget.hpp"

#include "../core/book.hpp"
#include "../core/borrowing_history.hpp"
#include "../core/borrowing_service.hpp"

#include <memory>

struct SDL_Texture;

class UIBookView : public IUIAbstract {
private:
    SDL_Texture *imageTexture;
    Book book;
    AppContext& appContext;
    bool departmentRestrictionActivate = false;
    bool ageRestrictionActivate = false;
    bool borrowConfirmationTick = false;

    std::unique_ptr<FavoriteBookRefreshEvent> refreshEvent = std::make_unique<FavoriteBookRefreshEvent>();

    void drawImpl();

public:
    UIBookView(AppContext& ctx, const Book& tbook);
    void draw() override;
};