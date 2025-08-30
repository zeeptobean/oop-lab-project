#pragma once

#include <string>
#include <set>
#include <deque>
#include <cstdint>
#include <map>
#include <memory>

#include "borrowing_policy.hpp"
#include "timestamp.hpp"

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

    User();
    User(const User& other);

    const IBorrowingPolicy* getBorrowingPolicy() const;
    uint64_t getInternalId() const;

    virtual std::unique_ptr<User> clone() const;
    virtual ~User();
};

class UniversityUser : public User {
    public:
    std::string workId;
    std::string department;

    UniversityUser();
    UniversityUser(const UniversityUser& other);

    virtual std::unique_ptr<User> clone() const override;
};