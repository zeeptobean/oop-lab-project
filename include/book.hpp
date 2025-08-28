#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <set>
#include <timestamp.hpp>
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

class Book {
    public:
    uint64_t internalId;
    std::string imagePath;
    std::string title;
    std::vector<std::string> authors;
    uint32_t publicationYear;
    uint32_t ageRating;
    uint64_t isbn;
    std::vector<std::string> tags;
    std::vector<std::string> departmentRestriction;
    Timestamp addedTimestamp;

    std::string getAuthorsString() const {
        std::string result;
        for(size_t i=0; i < authors.size(); i++) {
            result += authors[i];
            if(i != authors.size() - 1) {
                result += ", ";
            }
        }
        return result;
    }

    std::string getTagsString() const {
        std::string result;
        for(size_t i=0; i < tags.size(); i++) {
            result += tags[i];
            if(i != tags.size() - 1) {
                result += ", ";
            }
        }
        return result;
    }
};

class BookDatabase {
    private:
    void updateBook(Book& book, const json& jsonData) {
        book.internalId = jsonData.at("id");
        book.imagePath = jsonData.at("imagePath");
        book.title = jsonData.at("title");
        book.authors = jsonData.at("authors");
        book.publicationYear = jsonData.at("publicationYear");
        book.ageRating = jsonData.at("ageRating");
        book.isbn = jsonData.at("isbn");
        book.tags = jsonData.at("tags");
        book.departmentRestriction = jsonData.at("departmentRestriction");
        book.addedTimestamp = Timestamp(jsonData.at("addedTimestamp"));
    }

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

    bool loadFile(const std::string& filename) {
        std::vector<Book> tempBookVec;
        std::ifstream jsonFile(filename, std::ios::in | std::ios::out | std::ios::app);
        if(!jsonFile) {
            return false;
        }

        json jsonData;
        try {
            jsonFile >> jsonData;
            for(auto& item : jsonData.at("books")) {
                Book tbook;
                updateBook(tbook, item);
                if(uniqueIdSet.find(tbook.internalId) != uniqueIdSet.end()) {
                    std::cout << "book id " << tbook.internalId << " duplicated, skipping\n";
                    continue;
                }
                uniqueIdSet.insert(tbook.internalId);
                tempBookVec.emplace_back(tbook);
            }
        } catch(std::exception& e) {
            return false;
        }

        bookVec = std::move(tempBookVec);
        jsonFile.close();
        return true;
    }

    const std::vector<Book>& getAllBooks() {
        return bookVec;
    }

    std::vector<Book*> getLatestBooks(size_t count) {
        size_t n = std::min(count, bookVec.size());
        std::vector<Book*> result(bookVec.size(), nullptr);
        for(int i=0; i < bookVec.size(); i++) {
            result[i] = &bookVec[i];
        }
        std::sort(result.begin(), result.end(), [](Book* a, Book* b) {
            return a->addedTimestamp > b->addedTimestamp;
        });
        result.resize(n);
        return result;
    }

    std::vector<Book*> getBooksByAuthor(const std::string& author) {
        std::vector<Book*> result;
        for(auto& book : bookVec) {
            if(std::find(book.authors.begin(), book.authors.end(), author) != book.authors.end()) {
                result.push_back(&book);
            }
        }
        return result;
    }

    private:
    BookDatabase() = default;
    ~BookDatabase() = default;

    std::vector<Book> bookVec;
    std::set<uint64_t> uniqueIdSet;
};
