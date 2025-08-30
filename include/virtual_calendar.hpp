#pragma once

#include "core/timestamp.hpp"
#include "event_dispatcher.hpp"
#include "event_concrete.hpp"
#include <memory>

class VirtualCalendar {
    public:
    Timestamp currentTime;

    static VirtualCalendar& get() {
        static VirtualCalendar instance;
        return instance;
    }

    VirtualCalendar(const VirtualCalendar&) = delete;
    VirtualCalendar& operator=(const VirtualCalendar&) = delete;

    void advanceDay(int days) {
        currentTime = currentTime.advanceDay(days);
        EventDispatcher::get().dispatchEvent(refreshEvent.get());
    }
    
    private:
    std::unique_ptr<BorrowingHistoryRefreshEvent> refreshEvent = std::make_unique<BorrowingHistoryRefreshEvent>();
    VirtualCalendar() = default;
    ~VirtualCalendar() = default;
};