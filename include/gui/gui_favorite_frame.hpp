#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
#include "../app_context.hpp"
#include "../event_dispatcher.hpp"
#include "gui_book_catalogue.hpp"
#include <memory>

class UIFavoriteStatusFrame : public IUIAbstract, Observer {
    private:
    AppContext& appContext;
    std::unique_ptr<UIBookCatalogue> bookCatalogue = nullptr;
    std::vector<Book*> incomingBookVec, bookVec;
    bool refreshSignal = false;

    void refreshData();

    public:
    UIFavoriteStatusFrame(AppContext& context) : Observer(), appContext(context) {
        refreshData();
    }

    void draw() override;
    void onEvent(const Event *event) override {
        if(dynamic_cast<const FavoriteBookRefreshEvent*>(event)) {
            refreshData();
        }
    }
};