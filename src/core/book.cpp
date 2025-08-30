#include "core/book.hpp"

std::string Book::getAuthorsString() const {
    std::string result;
    for(size_t i = 0; i < authors.size(); i++) {
        result += authors[i];
        if(i != authors.size() - 1) {
            result += ", ";
        }
    }
    return result;
}

std::string Book::getTagsString() const {
    std::string result;
    for(size_t i = 0; i < tags.size(); i++) {
        result += tags[i];
        if(i != tags.size() - 1) {
            result += ", ";
        }
    }
    return result;
}