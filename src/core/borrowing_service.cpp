#include "core/borrowing_service.hpp"

using json = nlohmann::json;

BorrowingService::BorrowingService() {
    refreshEvent = std::make_unique<BorrowingHistoryRefreshEvent>();
}

BorrowingService::~BorrowingService() {}

void BorrowingService::initCurrentCountMap() {
    userCurrentBorrowedCountMap.clear();
    for(const auto& [userId, histories] : userBorrowingHistoryMap) {
        int count = 0;
        for(const auto& history : histories) {
            if(history.action == "borrow") {
                count++;
            } else if(history.action == "return") {
                count--;
            }
        }
        userCurrentBorrowedCountMap[userId] = count;
    }
}

BorrowingService& BorrowingService::get() {
    static BorrowingService instance;
    return instance;
}

int BorrowingService::borrowBook(uint64_t userId, uint64_t bookId) {
    User *user = UserDatabase::get().query(userId);
    if(!user) return -1;
    uint32_t bookCount = queryBookCount(bookId);
    if(bookCount <= 0) {
        return -2;
    }
    if(user->isSuspended) {
        return -3;
    }
    int userLimit = user->getBorrowingPolicy()->getMaxLoanAllowed();
    if(queryUserCurrentBorrowedCount(userId) >= userLimit) {
        return -4;
    }

    userCurrentBorrowedCountMap[userId]++;
    bookCountMap[bookId]--;
    userBorrowingHistoryMap[userId].emplace_back("borrow", bookId, userId, Timestamp::now());
    EventDispatcher::get().dispatchEvent(refreshEvent.get());
    return 1;
}

bool BorrowingService::returnBook(uint64_t userId, uint64_t bookId) {
    if(!queryUserBorrowingBook(userId, bookId)) return false;
    bookCountMap[bookId]++;
    userBorrowingHistoryMap[userId].emplace_back("return", bookId, userId, Timestamp::now());
    EventDispatcher::get().dispatchEvent(refreshEvent.get());
    return true;
}

bool BorrowingService::loadBorrowingHistory(const std::string& filename) {
    borrowingHistoryFile = filename;
    std::ifstream infile(filename);
    if(!infile.is_open()) return false;
    json j;

    try {
        infile >> j;
        j = j.at("borrowing_history");
        for(const auto& item : j) {
            uint64_t userId = item.at("user_id").get<uint64_t>();
            uint64_t bookId = item.at("book_id").get<uint64_t>();
            Timestamp timestamp = Timestamp(item.at("timestamp"));
            std::string action = item.at("action").get<std::string>();
            userBorrowingHistoryMap[userId].emplace_back(action, bookId, userId, timestamp);
        }
    } catch(const std::exception& e) {
        return false;
    }
    infile.close();
    initCurrentCountMap();
    return true;
}

bool BorrowingService::loadBookStock(const std::string& filename) {
    bookStockFile = filename;
    std::ifstream infile(filename);
    if(!infile.is_open()) return false;
    json j;

    try {
        infile >> j;
        const auto& stockArray = j.at("book_stock");
        for (const auto& item : stockArray) {
            for (const auto& [idString, count] : item.items()) {
                uint64_t bookId = std::stoull(idString);
                bookCountMap[bookId] = count.get<int>();
            }
        }
    } catch(const std::exception& e) {
        return false;
    }
    infile.close();
    return true;
}

int BorrowingService::queryBookCount(uint64_t bookId) {
    auto it = bookCountMap.find(bookId);
    if(it != bookCountMap.end()) {
        return it->second;
    }
    return 0;
}

bool BorrowingService::queryUserBorrowingBook(uint64_t userId, uint64_t bookId) {
    auto it = userBorrowingHistoryMap.find(userId);
    if(it != userBorrowingHistoryMap.end()) {
        std::set<uint64_t> tempSet;
        for(const auto& history : it->second) {
            if(history.action == "borrow") {
                tempSet.insert(history.bookId);
            } else if(history.action == "return") {
                if(tempSet.find(history.bookId) != tempSet.end()) {
                    tempSet.erase(history.bookId);
                }
            }
        }
        return tempSet.find(bookId) != tempSet.end();
    }
    return false;
}

int BorrowingService::queryUserCurrentBorrowedCount(uint64_t userId) {
    auto it = userCurrentBorrowedCountMap.find(userId);
    if(it != userCurrentBorrowedCountMap.end()) {
        return it->second;
    }
    return 0;
}

std::vector<BorrowingHistory> BorrowingService::queryUserBorrowingHistory(uint64_t userId) {
    auto it = userBorrowingHistoryMap.find(userId);
    if(it != userBorrowingHistoryMap.end()) {
        return it->second;
    }
    return {};
}

void BorrowingService::writeBookStock() {
    std::ofstream outfile(bookStockFile);
    if(!outfile.is_open()) return;
    json j;
    j["book_stock"] = json::array();
    for(const auto& [bookId, count] : bookCountMap) {
        j["book_stock"].push_back({
            {std::to_string(bookId), count}
        });
    }
    outfile << j.dump(4);
    outfile.close();
}

void BorrowingService::writeBorrowingHistory() {
    std::ofstream outfile(borrowingHistoryFile);
    if(!outfile.is_open()) return;
    json j;
    j["borrowing_history"] = json::array();
    for(const auto& [userId, histories] : userBorrowingHistoryMap) {
        for(const auto& history : histories) {
            j["borrowing_history"].push_back({
                {"user_id", history.userId},
                {"book_id", history.bookId},
                {"timestamp", history.time.toString()},
                {"action", history.action}
            });
        }
    }
    outfile << j.dump(4);
    outfile.close();
}