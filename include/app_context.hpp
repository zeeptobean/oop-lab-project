#pragma once
#include <functional>
#include <memory>
#include <string>
#include "gui_abstract.hpp"
#include "user.hpp"

// Forward declare SDL_Renderer
struct SDL_Renderer;

struct AppContext {
    SDL_Renderer* const renderer;
    User *currentUser = nullptr;

    using AddTabPageFunc = std::function<void(std::unique_ptr<IUIAbstract> content, const std::string& title)>;
    const AddTabPageFunc requestNewTab;
    
    AppContext(SDL_Renderer* trender, AddTabPageFunc tabFunc)
        : renderer(trender), requestNewTab(std::move(tabFunc)) {}
};