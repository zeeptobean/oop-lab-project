#include "core/user.hpp"

User::User() = default;
User::User(const User& other) = default;

const IBorrowingPolicy* User::getBorrowingPolicy() const {
    return borrowingPolicy;
}

uint64_t User::getInternalId() const {
    return internalId;
}

std::unique_ptr<User> User::clone() const {
    return std::make_unique<User>(*this);
}

User::~User() = default;

UniversityUser::UniversityUser() = default;
UniversityUser::UniversityUser(const UniversityUser& other) = default;

std::unique_ptr<User> UniversityUser::clone() const {
    return std::make_unique<UniversityUser>(*this);
}