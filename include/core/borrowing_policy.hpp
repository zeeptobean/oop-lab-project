#pragma once

class IBorrowingPolicy {
protected:
    int loanDuration; // in days
    int maxLoanAllowed;
    double baseFine; // per day
public:
    int getLoanDuration() const;
    int getMaxLoanAllowed() const;
    virtual double calculateFine(int daysLate) const = 0;
    virtual ~IBorrowingPolicy();
};

class NormalBorrowingPolicy : public IBorrowingPolicy {
public:
    NormalBorrowingPolicy();
    double calculateFine(int daysLate) const override;
};

class UndergraduateBorrowingPolicy : public IBorrowingPolicy {
public:
    UndergraduateBorrowingPolicy();
    double calculateFine(int daysLate) const override;
};

class PostgraduateBorrowingPolicy : public IBorrowingPolicy {
public:
    PostgraduateBorrowingPolicy();
    double calculateFine(int daysLate) const override;
};

class PremiumBorrowingPolicy : public IBorrowingPolicy {
public:
    PremiumBorrowingPolicy();
    double calculateFine(int daysLate) const override;
};

// flyweight pattern
class BorrowingPolicyFactory {
public:
    static const IBorrowingPolicy* getNormalPolicy();
    static const IBorrowingPolicy* getUndergraduatePolicy();
    static const IBorrowingPolicy* getPostgraduatePolicy();
    static const IBorrowingPolicy* getPremiumPolicy();
};