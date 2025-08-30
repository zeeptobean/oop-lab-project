#pragma once

#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "gui_book_catalogue.hpp"
#include "app_context.hpp"
#include "core/book_database.hpp"
#include <functional>
#include <string>
#include <vector>
#include <memory>

class UISearchBar : public IUIAbstract {
    private:
    AppContext& appContext;
    std::string searchText = "";
    std::function<void(const std::string&)> onSearchCallback;

    public:
    UISearchBar(AppContext& context, std::function<void(const std::string&)> onSearch);

    void draw() override;

    const std::string& getSearchText() const;

    void clearSearch();

    void setSearchText(const std::string& text);
};

class UISearch : public IUIAbstract {
    private:
    AppContext& appContext;
    UISearchBar searchBar;
    std::vector<Book*> searchResult, incomingSearchResult;
    std::unique_ptr<UIBookCatalogue> bookCatalogue;

    public:
    UISearch(AppContext& context, std::string initialQuery = "");

    void draw() override;

    private:
    void performSearch(const std::string& query);
};