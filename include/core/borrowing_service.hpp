#pragma once

#include <fstream>
#include <set>
#include <sstream>
#include <utility>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "borrowing_history.hpp"
#include "borrowing_policy.hpp"
#include "user_database.hpp"
#include "timestamp.hpp"
#include "book.hpp"

#include "../event_dispatcher.hpp"
#include "../event_concrete.hpp" 

#include "../nlohmann/json.hpp"

class BorrowingService {
    private:
    std::map<uint64_t, std::vector<BorrowingHistory>> userBorrowingHistoryMap;
    std::map<uint64_t, int> userCurrentBorrowedCountMap;
    std::map<uint64_t, int> bookCountMap;
    std::string bookStockFile, borrowingHistoryFile;
    std::unique_ptr<BorrowingHistoryRefreshEvent> refreshEvent;

    BorrowingService();
    ~BorrowingService();

    void initCurrentCountMap();

    public:
    static BorrowingService& get();
    BorrowingService(const BorrowingService&) = delete;
    BorrowingService& operator=(const BorrowingService&) = delete;

    int borrowBook(uint64_t userId, uint64_t bookId);
    bool returnBook(uint64_t userId, uint64_t bookId);
    bool loadBorrowingHistory(const std::string& filename);
    bool loadBookStock(const std::string& filename);
    int queryBookCount(uint64_t bookId);
    bool queryUserBorrowingBook(uint64_t userId, uint64_t bookId);
    int queryUserCurrentBorrowedCount(uint64_t userId);
    std::vector<BorrowingHistory> queryUserBorrowingHistory(uint64_t userId);
    void writeBookStock();
    void writeBorrowingHistory();
};