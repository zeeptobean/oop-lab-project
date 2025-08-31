#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "gui_register_view.hpp"
#include "../app_context.hpp"
#include "../core/user_database.hpp"
#include "memory"

class UILoginView : public IUIAbstract {
    private:
    bool isLoginOpen = true;
    bool forgotPassword = false;
    AppContext& appContext;
    std::string email, password;
    unsigned int wrongAttempts = 0;

    User r_user;
    std::string dobStr, passwordStr, passwordStr2;

    std::unique_ptr<UIRegisterView> registerView = std::make_unique<UIRegisterView>();

    public:

    UILoginView() = default;
    UILoginView(AppContext& ctx) : appContext(ctx) {  }

    void resetLogin();

    void draw() override;
};