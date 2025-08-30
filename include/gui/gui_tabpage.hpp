#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "../app_context.hpp"

#include <string>
#include <memory>

class UITabPage : public IUIAbstract {
    private:
    bool closable = true;
    bool active = true;
    bool *ptr = nullptr;
    std::string title;
    std::string unique_title;
    public:
    std::unique_ptr<IUIAbstract> content;

    UITabPage(std::unique_ptr<IUIAbstract> tcontent, const std::string& ttitle, bool isClosable = true);

    UITabPage(UITabPage&& other);

    // Move Assignment Operator
    UITabPage& operator=(UITabPage&& other) noexcept;

    UITabPage(const UITabPage&) = delete;
    UITabPage& operator=(const UITabPage&) = delete;

    void draw() override;

    bool isActive() const;
    virtual ~UITabPage() {};
};