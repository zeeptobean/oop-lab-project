
#include "core/book_database.hpp"
using json = nlohmann::json;

void BookDatabase::updateBook(Book& book, const json& jsonData) {
    try {
        book.internalId = jsonData.at("id");
        book.imagePath = (jsonData.contains("imagePath")) ? jsonData.at("imagePath") : "";
        book.title = jsonData.at("title");
        book.authors = jsonData.at("authors");
        book.publicationYear = jsonData.at("publicationYear");
        book.ageRating = (jsonData.contains("ageRating")) ? (uint32_t) jsonData.at("ageRating") : 0;
        book.isbn = jsonData.at("isbn");
        if(jsonData.contains("tags")) {
            book.tags = (std::vector<std::string>) jsonData.at("tags");
        } else {
            book.tags = std::vector<std::string>();
        }
        if(jsonData.contains("departmentRestriction")) {
            book.departmentRestriction = (std::vector<std::string>) jsonData.at("departmentRestriction");
        } else {
            book.departmentRestriction = std::vector<std::string>();
        }
    } catch(std::exception& e) {
        return;
    }
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

//query and search are different!
std::vector<Book*> BookDatabase::search(const std::vector<std::string>& keywords) {
    std::vector<Book*> result;
    const auto& books = BookDatabase::get().getAllBooks();
    for(const auto& book : books) {
        bool match = false;
        for(const auto& word : keywords) {
            if(toLowerStr(book.getAuthorsString()).find(word) != std::string::npos) {
                match = true;
                break;
            }
            if(toLowerStr(book.getTagsString()).find(word) != std::string::npos) {
                match = true;
                break;
            }
            if(toLowerStr(book.title).find(word) != std::string::npos) {
                match = true;
                break;
            }
            if(book.isbn.find(word) != std::string::npos) {
                match = true;
                break;
            }
        }
        if(match) {
            result.push_back(this->query(book.internalId)); //inefficient but safest
        }
    }
    return result;
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