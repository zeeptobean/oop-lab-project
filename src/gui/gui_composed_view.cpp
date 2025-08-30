#include "gui/gui_composed_view.hpp"

void UIComposedView::defaultDrawFunc() {
    for(auto& view : views) {
        view->draw();
    }
}

UIComposedView::UIComposedView(AppContext& context) : appContext(context), customDrawFunc(std::bind(&UIComposedView::defaultDrawFunc, this)) {}

void UIComposedView::addView(std::unique_ptr<IUIAbstract> view) {
    views.emplace_back(std::move(view));
}

void UIComposedView::draw() {
    customDrawFunc();
}