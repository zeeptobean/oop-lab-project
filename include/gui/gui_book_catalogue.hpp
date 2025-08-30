#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "gui_bookcard.hpp"
#include "../app_context.hpp"
#include "../core/book.hpp"

#include <vector>
#include <memory>

class UIBookCatalogue : public IUIAbstract {
    private:
    AppContext& appContext;
    std::vector<std::unique_ptr<UIBookCard>> bookCards;
    public:
    UIBookCatalogue(AppContext& context, const std::vector<Book*>& tbooks);
    void draw() override;
};