#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "gui_carousel_card.hpp"
#include "../core/book.hpp"
#include "../core/book_database.hpp"
#include "../app_context.hpp"
#include "nlohmann/json.hpp"
#include <vector>
#include <fstream>
#include <string>

class UICarouselView : public IUIAbstract {
private:
    AppContext& appContext;
    std::string headerStr;
    std::vector<UICarouselCard> bookCards; // Owns the cards directly
    const int cardHeight = 350;

public:
    explicit UICarouselView(AppContext&, const nlohmann::json& carouselConfig);
    void draw() override;
};