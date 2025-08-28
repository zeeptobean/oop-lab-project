#pragma once

#include <string>
#include <ctime>
#include <cstring>
#include <cstdio>

class Timestamp {
    private:
    tm *ctmtime;
    unsigned int day, month, year, hour, minute;

    int compare(const Timestamp& rhs) const {
        if (year != rhs.year) return year - rhs.year;
        if (month != rhs.month) return month - rhs.month;
        if (day != rhs.day) return day - rhs.day;
        if (hour != rhs.hour) return hour - rhs.hour;
        return minute - rhs.minute;
    }

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
    Timestamp(const std::string& isoString) {
        sscanf(isoString.c_str(), "%4u-%2u-%2uT%2u-%2u", &year, &month, &day, &hour, &minute);
    }
    std::string toString() const {
        char buf[201];
        memset(buf, 0, sizeof buf);
        snprintf(buf, 200, "%.4u-%.2u-%.2uT%.2u-%.2u", year, month, day, hour, minute);
        return std::string(buf);
    }
    std::string toLangString(const std::string& lang="en") const {
        const char* months_en[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        const char* months_fr[] = {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"};

        const char* *months = nullptr;
        if (lang == "fr") {
            months = months_fr;
        } else {
            months = months_en;
        }

        char buf[201];
        memset(buf, 0, sizeof buf);
        snprintf(buf, 200, "%s %02u, %.4u %02u:%02u", months[month], day, year, hour, minute);
        return std::string(buf);
    }
    ~Timestamp() {}

    bool operator<(const Timestamp& rhs) const {
        return compare(rhs) < 0;
    }
    bool operator==(const Timestamp& rhs) const {
        return compare(rhs) == 0;
    }
    bool operator>(const Timestamp& rhs) const {
        return compare(rhs) > 0;
    }
    bool operator<=(const Timestamp& rhs) const {
        return compare(rhs) <= 0;
    }
    bool operator>=(const Timestamp& rhs) const {
        return compare(rhs) >= 0;
    }
    bool operator!=(const Timestamp& rhs) const {
        return compare(rhs) != 0;
    }
};