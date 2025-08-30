#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "timestamp.hpp"

class Book {
    public:
    uint64_t internalId = 0;
    std::string imagePath;
    std::string title;
    std::vector<std::string> authors;
    uint32_t publicationYear;
    uint32_t ageRating;
    uint64_t isbn;
    std::vector<std::string> tags;
    std::vector<std::string> departmentRestriction;
    Timestamp addedTimestamp;

    std::string getAuthorsString() const;

    std::string getTagsString() const;
};
