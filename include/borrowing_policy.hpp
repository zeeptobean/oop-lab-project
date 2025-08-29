#pragma once

#include <cmath>
#include <cstdint>

class IBorrowingPolicy {
    protected:
    int loanDuration; // in days
    int maxLoanAllowed;
    double baseFine; // per day
    public:
    int getLoanDuration() const{
        return this->loanDuration;
    }
    int getMaxLoanAllowed() const {
        return this->maxLoanAllowed;
    }
    virtual double calculateFine(int daysLate) const = 0;
    virtual ~IBorrowingPolicy() {}
};

class NormalBorrowingPolicy : public IBorrowingPolicy {
    public:
    NormalBorrowingPolicy() {
        this->loanDuration = 14;
        this->maxLoanAllowed = 2;
        this->baseFine = 5.0f;
    }
    double calculateFine(int daysLate) const override {
        return this->baseFine * std::pow(1.08, daysLate);
    }
};

class UndergraduateBorrowingPolicy : public IBorrowingPolicy {
    public:
    UndergraduateBorrowingPolicy() {
        this->loanDuration = 14;
        this->maxLoanAllowed = 3;
        this->baseFine = 8.0f;
    }
    double calculateFine(int daysLate) const override {
        return this->baseFine * daysLate;
    }
};

class PostgraduateBorrowingPolicy : public IBorrowingPolicy {
    public:
    PostgraduateBorrowingPolicy() {
        this->loanDuration = 21;
        this->maxLoanAllowed = 5;
        this->baseFine = 7.0f;
    }
    double calculateFine(int daysLate) const override {
        return this->baseFine * daysLate;
    }
};

class PremiumBorrowingPolicy : public IBorrowingPolicy {
    public:
    PremiumBorrowingPolicy() {
        this->loanDuration = 30;
        this->maxLoanAllowed = 7;
        this->baseFine = 10.0f;
    }
    double calculateFine(int daysLate) const override {
        return this->baseFine * std::pow(1.10, daysLate);
    }
};

//flyweight pattern
class BorrowingPolicyFactory {
public:
    static const IBorrowingPolicy* getNormalPolicy() {
        static const NormalBorrowingPolicy policy;
        return &policy;
    }
    static const IBorrowingPolicy* getUndergraduatePolicy() {
        static const UndergraduateBorrowingPolicy policy;
        return &policy;
    }
    static const IBorrowingPolicy* getPostgraduatePolicy() {
        static const PostgraduateBorrowingPolicy policy;
        return &policy;
    }
    static const IBorrowingPolicy* getPremiumPolicy() {
        static const PremiumBorrowingPolicy policy;
        return &policy;
    }
};