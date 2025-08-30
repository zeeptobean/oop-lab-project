#pragma once

#include "event_dispatcher.hpp"

class BorrowingHistoryRefreshEvent : public Event {
    public:
    BorrowingHistoryRefreshEvent() {}
};

class FavoriteBookRefreshEvent : public Event {
    public:
    FavoriteBookRefreshEvent() {}
};