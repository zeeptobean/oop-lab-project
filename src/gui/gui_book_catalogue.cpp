#include "gui/gui_book_catalogue.hpp"

UIBookCatalogue::UIBookCatalogue(AppContext& context, const std::vector<Book*>& tbooks) : appContext(context) {
    for(Book* book : tbooks) {
        if(appContext.currentUser->checkEligibleAge(book->ageRating) && appContext.currentUser->checkDepartmentRestriction(book->departmentRestriction)) {
            bookCards.emplace_back(std::make_unique<UIBookCard>(context, book));
        }
    }
}
void UIBookCatalogue::draw() {
    for(auto& card : bookCards) {
        card->draw();
    }
}