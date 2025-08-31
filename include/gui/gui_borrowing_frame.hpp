#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include <SDL3/SDL.h>
#include "../app_context.hpp"
#include "../texture_cache.hpp"
#include "../event_dispatcher.hpp"
#include "../event_concrete.hpp" 
#include "../core/borrowing_service.hpp"
#include "../core/borrowing_policy.hpp"
#include "../core/book_database.hpp"
#include "imgui_custom_widget.hpp"

class UIBorrowingStatusFrame : public IUIAbstract, Observer {
    private:
    AppContext& appContext;
    const IBorrowingPolicy *policy;

    struct PreparedData {
        SDL_Texture *imageTexture = nullptr;
        std::string bookTitle;
        uint64_t bookId;
        Timestamp start, endExpected, endActual;
        bool isOverdue = false;
        bool isReturned = false;
        float fine = 0.0f;
    };
    
    std::vector<PreparedData> dataVec, incomingDataVec;

    void refreshData();

    public:
    UIBorrowingStatusFrame(AppContext& context, const IBorrowingPolicy *tpolicy);

    virtual ~UIBorrowingStatusFrame() = default;

    void onEvent(const Event *event) override {
        if(dynamic_cast<const BorrowingHistoryRefreshEvent*>(event)) {
            refreshData();
        }
    }

    void draw() override;
};