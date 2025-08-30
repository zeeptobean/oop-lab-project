#pragma once

#include <functional>
#include <memory>
#include <string>
#include "gui/gui_abstract.hpp"
#include "core/user.hpp"

// Forward declaration
struct SDL_Renderer;
struct ImFont;
struct ImVec2;

struct AppContext {
    SDL_Renderer* const renderer;
    User *currentUser = nullptr;
    ImFont* font = nullptr;
    ImFont* boldfont = nullptr;
    bool darkMode = true; // true for dark mode, false for light mode
    ImVec2 windowSize;

    using AddTabPageFunc = std::function<void(std::unique_ptr<IUIAbstract> content, const std::string& title)>;
    const AddTabPageFunc requestNewTab;

    using LogoutFunc = std::function<void()>;
    const LogoutFunc requestLogout;

    AppContext(SDL_Renderer* trender, AddTabPageFunc tabFunc, LogoutFunc logoutFunc)
        : renderer(trender), requestNewTab(std::move(tabFunc)), requestLogout(std::move(logoutFunc)) {}
};