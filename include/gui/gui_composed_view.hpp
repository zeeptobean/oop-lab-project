#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "../app_context.hpp"

class UIComposedView : public IUIAbstract {
    private:
    std::vector<std::unique_ptr<IUIAbstract>> views;
    AppContext& appContext;

    void defaultDrawFunc();

    public:
    std::function<void(void)> customDrawFunc = nullptr;
    UIComposedView(AppContext& context);
    void addView(std::unique_ptr<IUIAbstract> view);
    void draw() override;
};