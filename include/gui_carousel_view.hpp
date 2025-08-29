#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "gui_carousel_card.hpp"
#include "book.hpp"
#include "app_context.hpp"
#include <vector>
#include <nlohmann/json.hpp>

class UICarouselView : public IUIAbstract {
private:
    AppContext& appContext;
    std::string headerStr;
    std::vector<UICarouselCard> bookCards; // Owns the cards directly
    const int cardHeight = 400;

public:
    explicit UICarouselView(AppContext&, const json& carouselConfig);
    void draw() override;
};

UICarouselView::UICarouselView(AppContext& ctx, const json& carouselConfigFilename) : appContext(ctx) {
    try {
        std::ifstream fileStream(carouselConfigFilename);
        if (!fileStream) {
            throw std::runtime_error("Could not open carousel configuration file.");
        }
        json carouselConfig;
        fileStream >> carouselConfig;
        carouselConfig = carouselConfig.at("carousel")[0];
        headerStr = carouselConfig.at("header");
        std::vector<uint64_t> bookIdList = carouselConfig.at("bookId").get<std::vector<uint64_t>>();

        // Pre-reserve memory for efficiency
        bookCards.reserve(bookIdList.size());

        for (uint64_t id : bookIdList) {
            Book tbook;
            if (BookDatabase::get().query(id, tbook)) {
                // Create cards directly in the vector
                bookCards.emplace_back(appContext, tbook, cardHeight);
            }
        }
    } catch(const std::exception& e) {
        // Handle parsing errors
        headerStr = "Error Loading Carousel";
    }
}

void UICarouselView::draw() {
    ImGui::PushFont(appContext.font, 24.0f);
    ImGui::Text(headerStr.c_str());
    ImGui::PopFont();

    const float cardSpacing = 10.0f;

    ImGui::PushID(this);
    if(ImGui::BeginChild("##CarouselView", ImVec2(-1, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        if (ImGui::BeginTable("CarouselGrid", (int) bookCards.size(), ImGuiTableFlags_ScrollX)) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, cardSpacing));
            for (auto& card : bookCards) {
                ImGui::TableNextColumn();
                card.draw();
            }
            ImGui::PopStyleVar();
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::PopID();
}