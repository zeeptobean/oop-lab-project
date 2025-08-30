#include "gui/gui_search.hpp"

UISearchBar::UISearchBar(AppContext& context, std::function<void(const std::string&)> onSearch)
        : appContext(context), onSearchCallback(onSearch) {}

void UISearchBar::draw() {
    ImGui::SetNextItemWidth(appContext.windowSize.x * 0.3f);
    ImGui::Text("Search books by title, author, or ISBN");
    if(ImGui::InputText("##search", &searchText, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ElideLeft)) {
        onSearchCallback(searchText);
        ImGui::SetKeyboardFocusHere(-1);
    }
    ImGui::SameLine();
    if(ImGui::Button("Search")) {
        onSearchCallback(searchText);
        ImGui::SetKeyboardFocusHere(-1);
    }
    if(ImGui::Button("Clear")) {
        clearSearch();
        onSearchCallback(searchText);
        ImGui::SetKeyboardFocusHere(-1);
    }
}

const std::string& UISearchBar::getSearchText() const {
    return searchText;
}

void UISearchBar::clearSearch() {
    searchText.clear();
}

void UISearchBar::setSearchText(const std::string& text) {
    searchText = text;
}


UISearch::UISearch(AppContext& context, std::string initialQuery)
    : appContext(context),
        searchBar(context, [this](const std::string& query) { performSearch(query); })
{
    if(!initialQuery.empty()) {
        searchBar.setSearchText(initialQuery);
        performSearch(initialQuery);
    }
}

void UISearch::draw() {
    if(!incomingSearchResult.empty()) {
        std::swap(searchResult, incomingSearchResult);
        incomingSearchResult.clear();
        bookCatalogue = std::make_unique<UIBookCatalogue>(appContext, searchResult);
    }

    ImGui::PushID(this);
    ImGui::BeginChild("Search Books", ImVec2(-1, -1));
    searchBar.draw();
    ImGui::Separator();
    if(bookCatalogue) bookCatalogue->draw();
    ImGui::EndChild();
    ImGui::PopID();
}

void UISearch::performSearch(const std::string& query) {
    std::vector<std::string> wordList {""};
    bool inQuotes = false;
    for (char c : query) {
        if (c == '\"') {
            inQuotes = !inQuotes;
            if (!inQuotes) {
                if (wordList.back() != "") {
                    wordList.push_back("");
                }
            }
            continue;
        }

        if (inQuotes) {
            wordList.back().push_back(c);
        } else {
            if (isspace(c) || (ispunct(c) && c != '-' && c != '.')) {
                if (wordList.back() != "") {
                    wordList.push_back("");
                }
            } else {
                wordList.back().push_back(c);
            }
        }
    }

    for(int i=0; i < wordList.size(); ) {
        if(wordList[i].empty() || wordList[i] == "") {
            wordList.erase(wordList.begin() + i);
        } else {
            wordList[i] = toLowerStr(wordList[i]);
            i++;
        }
    }
    if(wordList.empty()) {
        wordList = {""};
    }

    incomingSearchResult = BookDatabase::get().search(wordList);
}