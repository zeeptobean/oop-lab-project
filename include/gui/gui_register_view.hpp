#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "../app_context.hpp"
#include "../core/user.hpp"
#include "../core/user_database.hpp"
#include <string>

class UIRegisterView : public IUIAbstract {
    private:
    User user;
    bool isOpen = true;
    std::string dobStr, passwordStr, passwordStr2;
    public:

    UIRegisterView() = default;
    void draw() override;
    void reset();
};