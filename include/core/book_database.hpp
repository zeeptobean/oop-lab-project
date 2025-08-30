#pragma once

#include "book.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <set>

#include "../nlohmann/json.hpp"

class BookDatabase {
    private:
    void updateBook(Book& book, const nlohmann::json& jsonData);

    public:
    static BookDatabase& get() {
        static BookDatabase instance;
        return instance;
    }
    BookDatabase(const BookDatabase&) = delete;
    BookDatabase& operator=(const BookDatabase&) = delete;

    Book* query(uint64_t bookId);

    bool loadFile(const std::string& filename);

    const std::vector<Book>& getAllBooks();

    std::vector<Book*> getLatestBooks(size_t count);

    std::vector<Book*> getBooksByAuthor(const std::string& author);

    private:
    BookDatabase();
    ~BookDatabase() = default;

    std::vector<Book> bookVec;
    std::set<uint64_t> uniqueIdSet;
};
