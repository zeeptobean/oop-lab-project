#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include <SDL3/SDL.h>
#include "gui_abstract.hpp"
#include "gui_borrowing_frame.hpp"
#include "../app_context.hpp"
#include "../internal_utilities.hpp"
#include "../core/user.hpp"
#include "../core/user_database.hpp"
#include "../core/borrowing_service.hpp"

class UIUserProfile : public IUIAbstract {
    private:
    User *user;
    UniversityUser *uuser = nullptr;
    AppContext& appContext;
    SDL_Texture *imageTexture = nullptr;
    std::unique_ptr<UIBorrowingStatusFrame> borrowingStatusFrame;

    public:
    UIUserProfile(AppContext& context);

    void draw() override;
};