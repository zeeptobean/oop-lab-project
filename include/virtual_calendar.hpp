#pragma once

#include "core/timestamp.hpp"

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
    }
    
    private:
    VirtualCalendar() = default;
    ~VirtualCalendar() = default;
};