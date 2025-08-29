#pragma once

#include <cstdint>
#include <map>
#include <string>
#include "borrowing_history.hpp"
#include "timestamp.hpp"
#include "book.hpp"
#include "borrowing_policy.hpp"
#include "user.hpp"
#include "event_dispatcher.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

class BorrowingHistoryRefreshEvent : public Event {
    public:
    BorrowingHistoryRefreshEvent() {}
};

class BorrowingService {
    private:
    std::map<uint64_t, std::vector<BorrowingHistory>> userBorrowingHistoryMap;
    std::map<uint64_t, int> userCurrentBorrowedCountMap;
    std::map<uint64_t, int> bookCountMap;
    std::string bookStockFile, borrowingHistoryFile;
    std::unique_ptr<BorrowingHistoryRefreshEvent> refreshEvent = std::make_unique<BorrowingHistoryRefreshEvent>();

    BorrowingService() = default;
    ~BorrowingService() {};

    void initCurrentCountMap() {
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

    public:
    static BorrowingService& get() {
        static BorrowingService instance;
        return instance;
    }
    BorrowingService(const BorrowingService&) = delete;
    BorrowingService& operator=(const BorrowingService&) = delete;

    int borrowBook(uint64_t userId, uint64_t bookId) {
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
    bool returnBook(uint64_t userId, uint64_t bookId) {
        if(!queryUserBorrowingBook(userId, bookId)) return false;
        bookCountMap[bookId]++;
        userBorrowingHistoryMap[userId].emplace_back("return", bookId, userId, Timestamp::now());
        EventDispatcher::get().dispatchEvent(refreshEvent.get());
        return true;
    }
    bool loadBorrowingHistory(const std::string& filename) {
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

    bool loadBookStock(const std::string& filename) {
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

    int queryBookCount(uint64_t bookId) {
        auto it = bookCountMap.find(bookId);
        if(it != bookCountMap.end()) {
            return it->second;
        }
        return 0;
    }

    bool queryUserBorrowingBook(uint64_t userId, uint64_t bookId) {
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

    int queryUserCurrentBorrowedCount(uint64_t userId) {
        auto it = userCurrentBorrowedCountMap.find(userId);
        if(it != userCurrentBorrowedCountMap.end()) {
            return it->second;
        }
        return 0;
    }

    std::vector<BorrowingHistory> queryUserBorrowingHistory(uint64_t userId) {
        auto it = userBorrowingHistoryMap.find(userId);
        if(it != userBorrowingHistoryMap.end()) {
            return it->second;
        }
        return {};
    }

    void writeBookStock() {
        //write book stock
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
    
    void writeBorrowingHistory() {
        //write borrowing history
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
};