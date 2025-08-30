#include "gui/gui_bookcard.hpp"

UIBookCard::UIBookCard(AppContext& context, Book* tbook) : appContext(context), book(tbook) {
    texture = TextureCache::get().book_get_texture(book->internalId);
}

void UIBookCard::draw() {
    if(!book) return;
    ImGui::PushID(this); 
    ImGui::BeginChild("BookCard", ImVec2(-1, 240.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::ImageNonStretch(texture, 150.0f, 200.0f);
    ImGui::SameLine();
    ImGui::BeginChild("BookCardText", ImVec2(-1, -1), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PushFont(appContext.boldfont, 20.0f);
    ImGui::TextEllipsisLines(book->title, 2);
    ImGui::PopFont();
    ImGui::Text("Author(s): %s", book->getAuthorsString().c_str());
    if(ImGui::Button("View Details")) {
        appContext.requestNewTab(std::make_unique<UIBookView>(appContext, *book), "Viewing " + book->title);
    }
    ImGui::EndChild();
    ImGui::EndChild();
    ImGui::PopID();
}