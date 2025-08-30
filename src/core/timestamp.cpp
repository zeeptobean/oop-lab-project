#include "core/timestamp.hpp"

int Timestamp::compare(const Timestamp& rhs) const {
    if (year != rhs.year) return year - rhs.year;
    if (month != rhs.month) return month - rhs.month;
    if (day != rhs.day) return day - rhs.day;
    if (hour != rhs.hour) return hour - rhs.hour;
    return minute - rhs.minute;
}

Timestamp Timestamp::now() {
    return Timestamp();
}

Timestamp::Timestamp() {
    const time_t tm = time(NULL);
    ctmtime = localtime(&tm);
    this->hour = ctmtime->tm_hour;
    this->minute = ctmtime->tm_min;
    this->year = ctmtime->tm_year+1900;
    this->month = ctmtime->tm_mon;
    this->day= ctmtime->tm_mday;
}

Timestamp::Timestamp(const std::string& isoString) {
    sscanf(isoString.c_str(), "%4u-%2u-%2uT%2u-%2u", &year, &month, &day, &hour, &minute);
}

std::string Timestamp::toString() const {
    char buf[201];
    memset(buf, 0, sizeof buf);
    snprintf(buf, 200, "%.4u-%.2u-%.2uT%.2u-%.2u", year, month, day, hour, minute);
    return std::string(buf);
}

std::string Timestamp::toLangString(const std::string& lang) const {
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

uint64_t Timestamp::yearDiff(const Timestamp& rhs) const {
    int diff = year - rhs.year;
    if (month < rhs.month || (month == rhs.month && day < rhs.day)) {
        diff--;
    }
    return std::abs(diff);
}

uint64_t Timestamp::dayDiff(const Timestamp& rhs) const {
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int totalDays1 = year * 365 + day;
    for (int m = 0; m < month; ++m) {
        totalDays1 += daysInMonth[m];
    }
    int totalDays2 = rhs.year * 365 + rhs.day;
    for (int m = 0; m < rhs.month; ++m) {
        totalDays2 += daysInMonth[m];
    }
    return std::abs(totalDays1 - totalDays2);
}

Timestamp Timestamp::advanceDay(int days) const {
    tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month;
    t.tm_mday = day + days;
    t.tm_hour = hour;
    t.tm_min = minute;
    mktime(&t);
    char buf[201];
    memset(buf, 0, sizeof buf);
    snprintf(buf, 200, "%.4u-%.2u-%.2uT%.2u-%.2u", t.tm_year + 1900, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min);
    return Timestamp(std::string(buf));
}