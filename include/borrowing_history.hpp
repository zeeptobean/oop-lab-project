#pragma once

#include <cstdint>
#include <map>
#include "timestamp.hpp"
#include "book.hpp"

class BorrowingHistory {
    public:
    std::string action = 0;
    uint64_t bookId = 0;
    uint64_t userId = 0;
    Timestamp startTimestamp, endTimestamp;
    double fine = 0.0;
    BorrowingHistory(std::string action, uint64_t bookId, uint64_t userId, Timestamp startTimestamp, Timestamp endTimestamp, double fine)
        : action(action), bookId(bookId), userId(userId), startTimestamp(startTimestamp), endTimestamp(endTimestamp), fine(fine) {}
};

enum BookStatus {
    UNAVAILABLE = 0,
    AVAILABLE = 1,
    BORROWED = 2,
};

class BorrowingService {
    private:
    std::map<uint64_t, std::vector<BorrowingHistory>> userBorrowingHistoryMap;
    std::map<uint64_t, BookStatus> bookStatusMap;
    BorrowingService() = default;
    ~BorrowingService() = default;

    bool UserAccountStatusHandler(uint64_t userId) {
        // Check if the user account is active or suspended or age/department restricted
        return true; // Placeholder
    }

    bool UserLoanLimitHandler(uint64_t userId) {
        // Check if the user has reached their borrowing limit
        return true; // Placeholder
    }
    public:
    static BorrowingService& get() {
        static BorrowingService instance;
        return instance;
    }
    BorrowingService(const BorrowingService&) = delete;
    BorrowingService& operator=(const BorrowingService&) = delete;

    bool borrowBook(uint64_t userId, uint64_t bookId) {
        BookStatus status = queryBookStatus(bookId);
        if (status != BookStatus::AVAILABLE) {
            return false;
        }
        if (!UserAccountStatusHandler(userId)) {
            return false;
        }
        if (!UserLoanLimitHandler(userId)) {
            return false;
        }
        return true;
    }
    bool returnBook(uint64_t userId, uint64_t bookId) {
        return false;
    }
    BookStatus queryBookStatus(uint64_t bookId) {
        auto it = bookStatusMap.find(bookId);
        if(it != bookStatusMap.end()) {
            return it->second;
        }
        return BookStatus::UNAVAILABLE;
    }
};