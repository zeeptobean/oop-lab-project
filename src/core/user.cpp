#include "core/user.hpp"
#include <algorithm>

User::User() = default;
User::User(const User& other) = default;

const IBorrowingPolicy* User::getBorrowingPolicy() const {
    return borrowingPolicy;
}

uint64_t User::getInternalId() const {
    return internalId;
}

bool User::checkEligibleAge(int age) const {
    return this->dob.yearDiff(Timestamp::now()) >= age;
}

bool User::checkDepartmentRestriction(const std::vector<std::string>& dept) const {
    if(dept.empty()) return true;
    const UniversityUser* uuser = dynamic_cast<const UniversityUser*>(this);
    if(!uuser) return false;
    return std::find(dept.begin(), dept.end(), uuser->department) != dept.end();
}

bool User::checkFavoriteBook(uint64_t bookId) const {
    return favoriteBookId.find(bookId) != favoriteBookId.end();
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