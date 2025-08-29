#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
#include <string>

namespace ImGui {
void TextEllipsisLines(const std::string& str, int maxLines = 2) {
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
}