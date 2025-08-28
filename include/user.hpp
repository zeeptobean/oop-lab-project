#pragma once

#include <string>
#include <set>
#include <deque>
#include <cstdint>
#include <map>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "borrowing_policy.hpp"
#include "timestamp.hpp"
#include "../sha256.hpp"

using json = nlohmann::json;

class User {
    private:
    IBorrowingPolicy *borrowingPolicy;
    std::set<uint64_t> favoriteBookId;
    std::deque<uint64_t> recentBookId;
    uint64_t internalId;
    std::string passwordHash;
    public:
    std::string name;
    Timestamp dob;
    std::string nid;
    std::string email;
    std::string address;
    std::string phone;
    std::string image;

    friend class UserDatabase;
};

enum class UniversityUserType {
    UNDERGRADUATE,
    POSTGRADUATE,
    TEACHER,
    STAFF
};

class UniversityUser : public User {
    private:
    std::string workId;
    std::string department;
    UniversityUserType userType;
};

class UserDatabase {
    private:
    UserDatabase() = default;
    ~UserDatabase() = default;

    std::map<uint64_t, User> userMap;
    std::map<std::string, uint64_t> tokenToUserIdMap;

    std::string generateAccessToken() {
        const char alphanum[] = "abcdefghijklmnopqrstuvwxyz";
        std::string token(32, ' ');

        for(int i = 0; i < 32; i++) {
            token[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        return token;
    }

    void internalUpdateUserJson(User& user, const json& jsonData) {
        user.internalId = jsonData.at("internalId");
        user.name = jsonData.at("name");
        std::string dobString = jsonData.at("dob");
        user.dob = Timestamp(dobString);
        user.nid = jsonData.at("nid");
        user.email = jsonData.at("email");
        user.address = jsonData.at("address");
        user.phone = jsonData.at("phone");
        user.image = jsonData.at("image");
        user.passwordHash = jsonData.at("passwordHash");

        std::vector<uint64_t> favoriteBookVec = jsonData.at("favoriteBookId");
        std::vector<uint64_t> recentBookVec = jsonData.at("recentBookId");
        std::string userType = jsonData.at("type");
        user.favoriteBookId.clear();
        user.recentBookId.clear();
        for(auto& bookId : favoriteBookVec) {
            user.favoriteBookId.insert(bookId);
        }
        for(auto& bookId : recentBookVec) {
            user.recentBookId.push_back(bookId);
        }
        if(userType == "undergraduate") {
            user.borrowingPolicy = const_cast<IBorrowingPolicy*>(BorrowingPolicyFactory::getUndergraduatePolicy());
        } else if(userType == "postgraduate") {
            user.borrowingPolicy = const_cast<IBorrowingPolicy*>(BorrowingPolicyFactory::getPostgraduatePolicy());
        } else if(userType == "teacher" || userType == "premium") {
            user.borrowingPolicy = const_cast<IBorrowingPolicy*>(BorrowingPolicyFactory::getPremiumPolicy());
        } else if(userType == "staff") {
            user.borrowingPolicy = const_cast<IBorrowingPolicy*>(BorrowingPolicyFactory::getNormalPolicy());
        } else {
            user.borrowingPolicy = const_cast<IBorrowingPolicy*>(BorrowingPolicyFactory::getNormalPolicy());
        }
    }
    
    void internalUpdateUser(const User& user) {
        if(userMap.find(user.internalId) != userMap.end()) {
            userMap[user.internalId] = user;
        } else {
            std::cerr << "User ID " << user.internalId << " duplicated. Do not update" << std::endl;
        }
    }

    public:
    static UserDatabase& get() {
        static UserDatabase instance;
        return instance;
    }
    UserDatabase(const UserDatabase&) = delete;
    UserDatabase& operator=(const UserDatabase&) = delete;

    bool commitUser(User tuser, const std::string& accessToken) {
        auto userIdIter = tokenToUserIdMap.find(accessToken);
        if (userIdIter != tokenToUserIdMap.end()) {
            auto userId = userIdIter->second;
            tuser.internalId = userId;
            internalUpdateUser(tuser);
            return true;
        }
        return false;
    }

    bool login(const std::string& email, const std::string& password, std::string& outToken) {
        for(auto& [userId, user] : userMap) {
            if(user.email == email && user.passwordHash == sha256(password)) {
                outToken = generateAccessToken();
                tokenToUserIdMap[outToken] = userId;
                return true;
            }
        }
        return false;
    }
    bool logout(uint64_t accessToken) {
        for(auto& [token, userId] : tokenToUserIdMap) {
            if(userId == accessToken) {
                tokenToUserIdMap.erase(token);
                return true;
            }
        }
        return false;
    }

    User* getUserByToken(const std::string& accessToken) {
        for(auto& [token, userId] : tokenToUserIdMap) {
            if(token == accessToken) {
                auto it = userMap.find(userId);
                if (it != userMap.end()) {
                    return &it->second;
                }
                return nullptr;
            }
        }
        return nullptr;
    }

    bool loadFile(const std::string& filename) {
        std::ifstream jsonFile (filename, std::ios::in | std::ios::out | std::ios::app);
        if(!jsonFile) {
            return false;
        }

        json jsonData;
        try {
            jsonFile >> jsonData;
            for(auto& item : jsonData.at("users")) {
                User tuser;
                internalUpdateUserJson(tuser, item);
                internalUpdateUser(tuser);
            }
        } catch(std::exception& e) {
            return false;
        }
    }

    User* getUserById(const uint64_t& userId) {
        auto it = userMap.find(userId);
        if (it != userMap.end()) {
            return &it->second;
        }
        return nullptr;
    }
};