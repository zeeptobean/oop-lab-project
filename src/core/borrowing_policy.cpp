#include <cmath>

#include "core/borrowing_policy.hpp"

int IBorrowingPolicy::getLoanDuration() const {
    return this->loanDuration;
}

int IBorrowingPolicy::getMaxLoanAllowed() const {
    return this->maxLoanAllowed;
}

IBorrowingPolicy::~IBorrowingPolicy() = default;

NormalBorrowingPolicy::NormalBorrowingPolicy() {
    this->loanDuration = 14;
    this->maxLoanAllowed = 2;
    this->baseFine = 5.0;
}
double NormalBorrowingPolicy::calculateFine(int daysLate) const {
    return this->baseFine * std::pow(1.08, daysLate);
}

UndergraduateBorrowingPolicy::UndergraduateBorrowingPolicy() {
    this->loanDuration = 14;
    this->maxLoanAllowed = 3;
    this->baseFine = 8.0;
}
double UndergraduateBorrowingPolicy::calculateFine(int daysLate) const {
    return this->baseFine * daysLate;
}

PostgraduateBorrowingPolicy::PostgraduateBorrowingPolicy() {
    this->loanDuration = 21;
    this->maxLoanAllowed = 5;
    this->baseFine = 7.0;
}
double PostgraduateBorrowingPolicy::calculateFine(int daysLate) const {
    return this->baseFine * daysLate;
}

PremiumBorrowingPolicy::PremiumBorrowingPolicy() {
    this->loanDuration = 30;
    this->maxLoanAllowed = 7;
    this->baseFine = 10.0;
}
double PremiumBorrowingPolicy::calculateFine(int daysLate) const {
    return this->baseFine * std::pow(1.10, daysLate);
}

const IBorrowingPolicy* BorrowingPolicyFactory::getNormalPolicy() {
    static const NormalBorrowingPolicy policy;
    return &policy;
}

const IBorrowingPolicy* BorrowingPolicyFactory::getUndergraduatePolicy() {
    static const UndergraduateBorrowingPolicy policy;
    return &policy;
}

const IBorrowingPolicy* BorrowingPolicyFactory::getPostgraduatePolicy() {
    static const PostgraduateBorrowingPolicy policy;
    return &policy;
}

const IBorrowingPolicy* BorrowingPolicyFactory::getPremiumPolicy() {
    static const PremiumBorrowingPolicy policy;
    return &policy;
}