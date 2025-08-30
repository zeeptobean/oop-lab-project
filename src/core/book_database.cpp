
#include "core/book_database.hpp"
using json = nlohmann::json;

void BookDatabase::updateBook(Book& book, const json& jsonData) {
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

Book* BookDatabase::query(uint64_t bookId) {
    auto it = std::find_if(bookVec.begin(), bookVec.end(), [&](const Book& book) {
        return book.internalId == bookId;
    });
    if (it != bookVec.end()) {
        return &(*it);
    }
    return &bookVec[0]; //return default book
}

bool BookDatabase::loadFile(const std::string& filename) {
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

const std::vector<Book>& BookDatabase::getAllBooks() {
    return bookVec;
}

std::vector<Book*> BookDatabase::getLatestBooks(size_t count) {
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

std::vector<Book*> BookDatabase::getBooksByAuthor(const std::string& author) {
    std::vector<Book*> result;
    for(auto& book : bookVec) {
        if(std::find(book.authors.begin(), book.authors.end(), author) != book.authors.end()) {
            result.push_back(&book);
        }
    }
    return result;
}

BookDatabase::BookDatabase() {
    Book defaultBook;
    defaultBook.internalId = 0;
    uniqueIdSet.insert(0);
    bookVec.emplace_back(defaultBook);
};