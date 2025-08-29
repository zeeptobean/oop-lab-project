#pragma once
#include <string>
#include <cstdint>
#include "timestamp.hpp"

class BorrowingHistory {
    public:
    std::string action;
    uint64_t bookId = 0;
    uint64_t userId = 0;
    Timestamp time;
    BorrowingHistory(std::string action, uint64_t bookId, uint64_t userId, Timestamp time)
        : action(action), bookId(bookId), userId(userId), time(time) {}
};
