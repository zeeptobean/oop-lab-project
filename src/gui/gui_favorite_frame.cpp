#include "gui/gui_favorite_frame.hpp"

void UIFavoriteStatusFrame::refreshData() {
    refreshSignal = true;
    incomingBookVec.clear();
    std::set<uint64_t> tset = appContext.currentUser->favoriteBookId;
    for(auto it = tset.begin(); it != tset.end(); it++) {
        Book* book = BookDatabase::get().query(*it);
        if(book->internalId != 0) {
            incomingBookVec.push_back(book);
        }
    }
}

void UIFavoriteStatusFrame::draw() {
    if(refreshSignal) {
        std::swap(bookVec, incomingBookVec);
        incomingBookVec.clear();
        bookCatalogue = std::make_unique<UIBookCatalogue>(appContext, bookVec);
        refreshSignal = false;
    }

    if(bookCatalogue) {
        bookCatalogue->draw();
    }
}