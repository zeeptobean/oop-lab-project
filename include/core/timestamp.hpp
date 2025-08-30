#pragma once

#include <string>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstdint>

class Timestamp {
    private:
    tm *ctmtime;
    unsigned int day, month, year, hour, minute;

    int compare(const Timestamp& rhs) const;

    public:
    static Timestamp now();

    Timestamp();
    Timestamp(const std::string& isoString);
    std::string toString() const;
    std::string toLangString(const std::string& lang="en") const;
    ~Timestamp() {}

    uint64_t yearDiff(const Timestamp&) const;

    uint64_t dayDiff(const Timestamp&) const;

    Timestamp advanceDay(int) const;

    inline bool operator<(const Timestamp& rhs) const {
        return compare(rhs) < 0;
    }
    inline bool operator==(const Timestamp& rhs) const {
        return compare(rhs) == 0;
    }
    inline bool operator>(const Timestamp& rhs) const {
        return compare(rhs) > 0;
    }
    inline bool operator<=(const Timestamp& rhs) const {
        return compare(rhs) <= 0;
    }
    inline bool operator>=(const Timestamp& rhs) const {
        return compare(rhs) >= 0;
    }
    inline bool operator!=(const Timestamp& rhs) const {
        return compare(rhs) != 0;
    }
};