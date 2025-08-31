#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
#include <SDL3/SDL.h>
#include <string>

namespace ImGui {
inline void TextEllipsisLines(const std::string& str, int maxLines = 2) {
    if (maxLines <= 0) return;

    const float lineHeight = ImGui::GetTextLineHeight();
    const float maxHeight = lineHeight * maxLines;
    const float availableWidth = ImGui::GetContentRegionAvail().x;

    // Measure full wrapped text
    ImVec2 fullTextSize = ImGui::CalcTextSize(str.c_str(), nullptr, true, availableWidth);

    if (fullTextSize.y <= maxHeight + 0.5f) {
        // Text fits fully
        ImGui::TextWrapped("%s", str.c_str());
        return;
    }

    // Needs truncation
    const std::string ellipsis = "...";
    int left = 0, right = (int)str.size();
    int bestFit = 0;

    while (left <= right) {
        int mid = (left + right) / 2;
        std::string candidate = str.substr(0, mid) + ellipsis;

        ImVec2 size = ImGui::CalcTextSize(candidate.c_str(), nullptr, true, availableWidth);

        if (size.y <= maxHeight + 0.5f) {
            bestFit = mid;
            left = mid + 1; // try longer
        } else {
            right = mid - 1; // try shorter
        }
    }

    std::string truncated = str.substr(0, bestFit) + ellipsis;
    ImGui::TextWrapped("%s", truncated.c_str());
}

inline void ImageNonStretch(SDL_Texture* texture, float contentWidth, float contentHeight) {
    if(!texture) {
        //draw empty frame in case image failed to load
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 childPos = ImGui::GetCursorScreenPos();
        ImVec2 childEndPos = ImVec2(childPos.x + contentWidth, childPos.y + contentHeight);
        ImU32 borderColor = IM_COL32(20, 20, 20, 255);
        drawList->AddRect(childPos, childEndPos, borderColor, 12.0f, 0, 1.5f);
        ImGui::Dummy(ImVec2(contentWidth, contentHeight));
        return;
    }
    int image_width = texture->w, image_height = texture->h;

    float aspect_ratio = (float)image_width / (float)image_height;

    float new_width = contentWidth;
    float new_height = new_width / aspect_ratio;

    if (new_height > contentHeight) {
        new_height = contentHeight;
        new_width = new_height * aspect_ratio;
    }

    // Center the image
    float cursor_x = (contentWidth - new_width) * 0.5f;
    float cursor_y = (contentHeight - new_height) * 0.5f;
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + cursor_x, ImGui::GetCursorPosY() + cursor_y));

    ImGui::Image((ImTextureID)texture, ImVec2(new_width, new_height));
}



}