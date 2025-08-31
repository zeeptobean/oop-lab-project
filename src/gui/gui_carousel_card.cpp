#include "gui/gui_carousel_card.hpp"

void UICarouselCard::draw() {
    ImGui::PushID(book->internalId);

    const ImVec2 cardSize(250, cardHeight);
    const float rounding = 6.0f;
    const float cardPadding = 12.0f;
    const float textPadding = 16.0f;

    if (ImGui::BeginChild("##CardCanvas", cardSize, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar)) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(cardPadding, cardPadding));
        bool isHovered = ImGui::IsWindowHovered();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 cardTopLeft = ImGui::GetWindowPos();
        ImVec2 cardBottomRight = ImVec2(cardTopLeft.x + cardSize.x, cardTopLeft.y + cardSize.y);
        ImU32 bgColor = appContext.darkMode ? ImColor(40, 40, 40) : ImColor(230, 230, 230);
        ImVec4 bgColorVec = ImGui::ColorConvertU32ToFloat4(bgColor);
        ImU32 borderColor = isHovered ? (ImU32) (appContext.darkMode ? ImColor(230, 100, 100) : ImColor(40, 40, 180)) : bgColor;
        ImVec4 textColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
        ImVec4 authorColor = ImVec4(
            bgColorVec.x + (textColor.x - bgColorVec.x) * 0.70f,
            bgColorVec.y + (textColor.y - bgColorVec.y) * 0.70f,
            bgColorVec.z + (textColor.z - bgColorVec.z) * 0.70f,
            textColor.w
        );

        // border
        drawList->AddRectFilled(cardTopLeft, cardBottomRight, bgColor, rounding);
        drawList->AddRect(cardTopLeft, cardBottomRight, borderColor, rounding, 0, 1.5f);

        //Draw image with card padding
        const float imageHeight = 200.0f;
        const float imageWidth = cardSize.x - cardPadding * 2;
        float imageIndentX = (cardSize.x - imageWidth) / 2.0f;
        ImGui::SetCursorPos(ImVec2(imageIndentX, cardPadding));
        ImGui::ImageNonStretch(imageTexture, imageWidth, imageHeight, appContext.darkMode);
        
        //Text area
        ImGui::Dummy(ImVec2(0.0f, 3.0f));
        ImGui::SetCursorPosX(textPadding);

        float textContentWidth = cardSize.x - textPadding * 2;
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + textContentWidth);

        ImGui::PushFont(appContext.boldfont, 18.0f);
        ImGui::TextEllipsisLines(book->title, 2);
        ImGui::PopFont();
        ImGui::SetCursorPosX(textPadding);
        ImGui::PushStyleColor(ImGuiCol_Text, authorColor);
        ImGui::TextEllipsisLines(book->getAuthorsString(), 1);
        ImGui::PopStyleColor();
        ImGui::PopTextWrapPos();

        const char* buttonText = "More info";
        const float buttonHeight = 30.0f;
        ImGui::SetCursorPos(ImVec2(textPadding, cardSize.y - buttonHeight - textPadding));
        if (ImGui::Button(buttonText, ImVec2(textContentWidth, buttonHeight))) {
            appContext.requestNewTab(std::make_unique<UIBookView>(appContext, *book), "Viewing " + book->title);
        }

        ImGui::PopStyleVar();
    }

    ImGui::EndChild();
    ImGui::PopID();
}