#include <bits/stdc++.h>
using namespace std;

class Timestamp {
    private:
    tm *ctmtime;
    unsigned int day, month, year, hour, minute;

    public:
    Timestamp() {
        const time_t tm = time(NULL);
        ctmtime = localtime(&tm);
        this->hour = ctmtime->tm_hour;
        this->minute = ctmtime->tm_min;
        this->year = ctmtime->tm_year+1900;
        this->month = ctmtime->tm_mon;
        this->day= ctmtime->tm_mday;
    }
    std::string toString() {
        char buf[201];
        memset(buf, 0, sizeof buf);
        snprintf(buf, 200, "%.2u/%.2u/%.4u %.2u:%.2u", day, month, year, hour, minute);
        return std::string(buf);
    }
    std::string toIsoString() {
        char buf[201];
        memset(buf, 0, sizeof buf);
        snprintf(buf, 200, "%.4u-%.2u-%.2uT%.2u-%.2u", year, month, day, hour, minute);
        return std::string(buf);
    }
    ~Timestamp() {}

    bool operator<(const Timestamp& rhs) {
        return false;   //temp
    }
};

class IBorrowingPolicy {
    protected:
    int loanDuration; // in days
    int maxLoanAllowed;
    double baseFine; // per day
    public:
    int getLoanDuration() {
        return this->loanDuration;
    }
    int getMaxLoanAllowed() {
        return this->maxLoanAllowed;
    }
    virtual double calculateFine(int daysLate) = 0;
    virtual ~IBorrowingPolicy() {}
};

class NormalBorrowingPolicy : public IBorrowingPolicy {
    public:
    NormalBorrowingPolicy() {
        this->loanDuration = 14;
        this->maxLoanAllowed = 2;
        this->baseFine = 5.0f;
    }
    double calculateFine(int daysLate) override {
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
    double calculateFine(int daysLate) override {
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
    double calculateFine(int daysLate) override {
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
    double calculateFine(int daysLate) override {
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


class User { 
    private:
    IBorrowingPolicy *borrowingPolicy;
    std::set<uint64_t> favoriteBookId;
    std::deque<uint64_t> recentBookId;
    uint64_t internalId;
    string passwordHash;
    public:
    string name;
    string dob;
    string nid;
    string email;
    string address;
    string phone;
    string image;
};

class Student : public User {
    private:
    std::string studentId;
    std::string department;
};

class Lecturer : public User {
    private:
    std::string employeeId;
    std::string department;
    //premium borrowing policy
};

class BorrowingHistory {
    public:
    uint64_t actionId = 0;
    uint64_t bookId = 0;
    uint64_t userId = 0;
    Timestamp startTimestamp, endTimestamp;
    double fine = 0.0;
    BorrowingHistory(uint64_t actionId, uint64_t bookId, uint64_t userId, Timestamp startTimestamp, Timestamp endTimestamp, double fine)
        : actionId(actionId), bookId(bookId), userId(userId), startTimestamp(startTimestamp), endTimestamp(endTimestamp), fine(fine) {}
};

class Book {
    public:
    uint64_t internalId;
    std::string internalImagePath;
    std::string title;
    std::vector<std::string> authors;
    uint32_t publicationYear;
    uint32_t ageRating;
    uint64_t isbn;
    std::vector<std::string> keywords;
    std::vector<std::string> departmentRestriction;
    
};

class BookDatabase {
    public:
    static BookDatabase& get() {
        static BookDatabase instance;
        return instance;
    }
    BookDatabase(const BookDatabase&) = delete;
    BookDatabase& operator=(const BookDatabase&) = delete;

    bool query(uint64_t bookId, Book& retBook) {
        auto it = std::find_if(bookVec.begin(), bookVec.end(), [&](const Book& book) {
            return book.internalId == bookId;
        });
        if (it != bookVec.end()) {
            retBook = *it;
            return true;
        }
        return false;
    }

    private:
    BookDatabase() = default;
    ~BookDatabase() = default;

    std::vector<Book> bookVec;
};

enum BookStatus {
    UNAVAILABLE = 0,
    AVAILABLE = 1,
    BORROWED = 2,
};

class BorrowingService {
    private:
    std::map<uint64_t, std::vector<BorrowingHistory>> userBorrowingHistoryMap;
    std::map<uint64_t, BookStatus> bookStatusMap;   //state pattern
    BorrowingService() = default;
    ~BorrowingService() = default;

    bool BookAvailabilityHandler(uint64_t bookId) {
        auto it = bookStatusMap.find(bookId);
        if (it != bookStatusMap.end()) {
            return it->second == BookStatus::AVAILABLE;
        }
        return false;
    }

    bool UserAccountStatusHandler(uint64_t userId) {
        // Check if the user account is active or suspended or age/department restricted
        return true; // Placeholder
    }

    bool UserLoanLimitHandler(uint64_t userId) {
        // Check if the user has reached their borrowing limit
        return true; // Placeholder
    }
    public:
    static BorrowingService& get() {
        static BorrowingService instance;
        return instance;
    }
    BorrowingService(const BorrowingService&) = delete;
    BorrowingService& operator=(const BorrowingService&) = delete;

    bool borrowBook(uint64_t userId, uint64_t bookId) {
        if (!BookAvailabilityHandler(bookId)) {
            return false;
        }
        if (!UserAccountStatusHandler(userId)) {
            return false;
        }
        if (!UserLoanLimitHandler(userId)) {
            return false;
        }
        return true;
    }
    bool returnBook(uint64_t userId, uint64_t bookId) {
        return false;
    }
    bool queryBookStatus(uint64_t bookId) {
        return false;
    }
};

class UserDatabase {
    private:
    UserDatabase() = default;
    ~UserDatabase() = default;
    public:
    static UserDatabase& get() {
        static UserDatabase instance;
        return instance;
    }
    UserDatabase(const UserDatabase&) = delete;
    UserDatabase& operator=(const UserDatabase&) = delete;

    bool commitUser(const uint64_t& userId, const std::string& accessToken) {
        return true;
    }

    bool login(const std::string& email, const std::string& password, std::string& outToken) {
        return true;
    }
    bool logout(uint64_t accessToken) {
        return true;
    }

    bool getUserByToken(const std::string& accessToken, User& outUser) {
        return true;
    }

    User* getUserById(const uint64_t& userId) {
        return nullptr;
    }
};

class RecommendationService {
    private:
    RecommendationService() = default;
    ~RecommendationService() = default;
    public:
    static RecommendationService& get() {
        static RecommendationService instance;
        return instance;
    }
    RecommendationService(const RecommendationService&) = delete;
    RecommendationService& operator=(const RecommendationService&) = delete;

    std::vector<uint64_t> getRecommendations(uint64_t userId) {
        return std::vector<uint64_t>();
    }
};

//Book and user are data snapshots
//service classes handle all the logic and data manipulation