#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "../app_context.hpp"
#include "../core/user_database.hpp"

class UILoginView : public IUIAbstract {
    private:
    bool isLoginOpen = true;
    AppContext& appContext;
    std::string email, password;
    unsigned int wrongAttempts = 0;
    public:

    UILoginView() = default;
    UILoginView(AppContext& ctx) : appContext(ctx) {  }

    void reset() {
        email = "";
        password = "";
        wrongAttempts = 0;
        isLoginOpen = true;
    }

    void draw() override;
};