#include "gui/gui_bookview.hpp"

//include here in source file to avoid circular dependencies in header
#include "gui/gui_search.hpp"

UIBookView::UIBookView(AppContext& ctx, const Book& tbook) : book(tbook), appContext(ctx) {
    imageTexture = TextureCache::get().book_get_texture(book.internalId);
    departmentRestrictionActivate = !appContext.currentUser->checkDepartmentRestriction(book.departmentRestriction);
    ageRestrictionActivate = !appContext.currentUser->checkEligibleAge(book.ageRating);
}

void UIBookView::drawImpl() {
    ImGui::PushID(this);
    ImGui::BeginGroup();
    ImGui::ImageNonStretch(imageTexture, 480, 640, appContext.darkMode);
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::PushFont(appContext.boldfont, 24.0f);
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetWindowWidth());
    ImGui::TextWrapped(book.title.c_str());
    ImGui::PopFont();
    ImGui::Text("ISBN: %s", book.isbn.c_str());
    ImGui::Text("Author(s): %s", book.getAuthorsString().c_str());
    ImGui::Text("Publication year: %s", book.publicationYear.c_str());
    if(book.ageRating == 0) {
        ImGui::Text("Age rating: EVERYONE");
    } else {
        ImGui::Text("Age rating: %u+", book.ageRating);
    }
    
    if(BorrowingService::get().queryUserBorrowingBook(appContext.currentUser->getInternalId(), book.internalId)) {
        ImGui::BeginDisabled();
        ImGui::Button("Borrow");
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(appContext.getColorGreen(), "You are borrowing this book");
    } else if(appContext.currentUser->isSuspended) {
        ImGui::BeginDisabled();
        ImGui::Button("Borrow");
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(appContext.getColorRed(), "Your account is suspended");
    } else if(BorrowingService::get().queryUserCurrentBorrowedCount(appContext.currentUser->getInternalId()) >= appContext.currentUser->getBorrowingPolicy()->getMaxLoanAllowed()) {
        ImGui::BeginDisabled();
        ImGui::Button("Borrow");
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(appContext.getColorRed(), "You have reached your borrowing limit");

    } else if(BorrowingService::get().queryBookCount(book.internalId) <= 0) {
        ImGui::BeginDisabled();
        ImGui::Button("Borrow");
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(appContext.getColorRed(), "Not available");
    } else {
        if(ImGui::Button("Borrow")) {
            ImGui::OpenPopup("Borrowing book confirmation");
        }
    }
    if(appContext.currentUser->checkFavoriteBook(book.internalId)) {
        if(ImGui::Button("Remove from Favorites")) {
            appContext.currentUser->favoriteBookId.erase(book.internalId);
            EventDispatcher::get().dispatchEvent(refreshEvent.get());
        }
    } else {
        if(ImGui::Button("Add to Favorites")) {
            appContext.currentUser->favoriteBookId.insert(book.internalId);
            EventDispatcher::get().dispatchEvent(refreshEvent.get());
        }
    }

    ImGui::Dummy(ImVec2(0, 15));
    for(const auto& authorName : book.authors) {
        if(ImGui::Button(std::string("More from " + authorName).c_str())) {
            std::string exactAuthorName = '"' + authorName + '"';
            appContext.requestNewTab(std::make_unique<UISearch>(appContext, exactAuthorName), "Search: " + authorName);
        }
    }
    if(ImGui::CollapsingHeader("Show tags")) {
        ImGui::PushFont(appContext.font, 14.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255)); //gray color
        ImGui::InputTextMultiline("##tags", (char*) book.getTagsString().c_str(), book.getTagsString().size()+3, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 8), ImGuiInputTextFlags_ReadOnly);
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }
    if (ImGui::BeginPopupModal("Borrowing book confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::BeginChild("##child", ImVec2(400, 200), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10,10));
        ImGui::TextWrapped("Are you sure to borrow %s ?", book.title.c_str());
        ImGui::Checkbox("I agreed to the borrowing policy and the library's terms of service", &borrowConfirmationTick);
        ImGui::BeginDisabled(!borrowConfirmationTick);
        if(ImGui::Button("Borrow")) {
            if(BorrowingService::get().borrowBook(appContext.currentUser->getInternalId(), book.internalId) > 0) {
                ImGui::CloseCurrentPopup(); 
                borrowConfirmationTick = false;
            } else {
                ImGui::TextColored(appContext.getColorRed(), "Failed to borrow book. Please try again later.");
            }
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if(ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::EndPopup();
    }
    ImGui::EndGroup();
    ImGui::PopID();
}

void UIBookView::draw() {
    if(departmentRestrictionActivate) {
        ImGui::PushFont(appContext.boldfont, 18.0f);
        ImGui::TextColored(appContext.getColorRed(), "You are not eligible to view this book due to department restriction.");
        ImGui::PopFont();
    } else if(ageRestrictionActivate) {
        ImGui::PushFont(appContext.boldfont, 18.0f);
        ImGui::TextColored(appContext.getColorRed(), "You are not eligible to view this book due to age restriction.");
        ImGui::PopFont();
    } else {
        drawImpl();
    }
}