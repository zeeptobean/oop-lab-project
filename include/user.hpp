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
#include "sha256.hpp"

using json = nlohmann::json;

class User {
    private:
    const IBorrowingPolicy *borrowingPolicy = nullptr;
    uint64_t internalId;
    std::string passwordHash;
    public:
    std::set<uint64_t> favoriteBookId;
    std::deque<uint64_t> recentBookId;
    std::string name;
    Timestamp dob;
    std::string nid;
    std::string email;
    std::string address;
    std::string phone;
    std::string image;
    std::string type;
    bool isSuspended = false;
    Timestamp suspensionEnd;

    friend class UserDatabase;

    User() = default;
    User(const User& other) = default;

    const IBorrowingPolicy* getBorrowingPolicy() const {
        return borrowingPolicy;
    }

    uint64_t getInternalId() const {
        return internalId;
    }

    virtual std::unique_ptr<User> clone() const {
        return std::make_unique<User>(*this);
    }
    virtual ~User() {}
};

class UniversityUser : public User {
    public:
    std::string workId;
    std::string department;

    UniversityUser() = default;
    UniversityUser(const UniversityUser& other) = default;

    virtual std::unique_ptr<User> clone() const override {
        return std::make_unique<UniversityUser>(*this);
    }
};

class UserDatabase {
    private:
    UserDatabase() = default;
    ~UserDatabase() = default;

    std::map<uint64_t, std::unique_ptr<User>> userMap;

    User* initUserJson(const json& jsonData) {
        if(!jsonData.contains("type")) {
            return nullptr;
        }
        User *user = nullptr;
        std::string userType = jsonData.at("type");
        if(userType == "undergraduate") {
            user = new UniversityUser();
            user->borrowingPolicy = BorrowingPolicyFactory::getUndergraduatePolicy();
        } else if(userType == "postgraduate") {
            user = new UniversityUser();
            user->borrowingPolicy = BorrowingPolicyFactory::getPostgraduatePolicy();
        } else if(userType == "teacher") {
            user = new UniversityUser();
            user->borrowingPolicy = BorrowingPolicyFactory::getPremiumPolicy();
        } else if(userType == "staff") {
            user = new UniversityUser();
            user->borrowingPolicy = BorrowingPolicyFactory::getNormalPolicy();
        } else if(userType == "premium") {
            user = new User();
            user->borrowingPolicy = BorrowingPolicyFactory::getPremiumPolicy();
        } else {
            user = new User();
            user->borrowingPolicy = BorrowingPolicyFactory::getNormalPolicy();
        }
        user->type = userType;

        try {
            user->internalId = jsonData.at("id");
            user->name = jsonData.at("name");
            user->dob = Timestamp((std::string) jsonData.at("dob"));
            user->nid = jsonData.at("nid");
            user->email = jsonData.at("email");
            user->address = jsonData.at("address");
            user->phone = jsonData.at("phone");
            user->image = jsonData.at("image");
            user->passwordHash = jsonData.at("passwordHash");
            user->isSuspended = jsonData.value("isSuspended", false);
            if(user->isSuspended) user->suspensionEnd = Timestamp((std::string) jsonData.at("suspendEnd"));

            std::vector<uint64_t> favoriteBookVec = jsonData.at("favoriteBookId");
            std::vector<uint64_t> recentBookVec = jsonData.at("recentBookId");
            user->favoriteBookId.clear();
            user->recentBookId.clear();
            for(auto& bookId : favoriteBookVec) {
                user->favoriteBookId.insert(bookId);
            }
            for(auto& bookId : recentBookVec) {
                user->recentBookId.push_back(bookId);
            }

            UniversityUser* uuser = dynamic_cast<UniversityUser*>(user);
            if(uuser) {
                uuser->workId = jsonData.at("workId");
                uuser->department = jsonData.at("department");
            }
        } catch(...) {
            delete user;
            return nullptr;
        }

        if(userMap.find(user->internalId) == userMap.end()) {
            userMap[user->internalId] = std::unique_ptr<User>(user);
        } else {
            std::cerr << "User ID " << user->internalId << " duplicated. Do not add" << std::endl;
            delete user;
            return nullptr;
        }

        return user;
    }

    public:
    static UserDatabase& get() {
        static UserDatabase instance;
        return instance;
    }
    UserDatabase(const UserDatabase&) = delete;
    UserDatabase& operator=(const UserDatabase&) = delete;

    User* login(const std::string& email, const std::string& password) {
        std::string passwordHash = sha256(password);
        for(auto const& [userId, userPtr] : userMap) {
            if(userPtr->email == email && userPtr->passwordHash == passwordHash) {
                return userPtr.get();
            }
        }
        return nullptr;
    }

    User* query(uint64_t userId) {
        auto it = userMap.find(userId);
        if(it != userMap.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    bool loadFile(const std::string& filename) {
        std::ifstream jsonFile(filename);
        if (!jsonFile) return false;

        json jsonData;
        try {
            jsonFile >> jsonData;
            for(auto& item : jsonData.at("users")) {
                User *newUser = initUserJson(item);
            }
        } catch (const std::exception& e) {
            return false;
        }
        return true;
    }
};