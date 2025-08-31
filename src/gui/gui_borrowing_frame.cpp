#include "gui/gui_borrowing_frame.hpp"

UIBorrowingStatusFrame::UIBorrowingStatusFrame(AppContext& context, const IBorrowingPolicy *tpolicy) : Observer(), appContext(context), policy(tpolicy) {
    refreshData();
}

void UIBorrowingStatusFrame::refreshData() {
    incomingDataVec.clear();
    std::map<uint64_t, Timestamp> borrowTimeMap;
    const auto& userBorrowingHistory = BorrowingService::get().queryUserBorrowingHistory(appContext.currentUser->getInternalId());
    for(auto ite=userBorrowingHistory.begin(); ite != userBorrowingHistory.end(); ite++) {
        const auto& history = *ite;
        if(history.action == "return" && borrowTimeMap.find(history.bookId) != borrowTimeMap.end()) {
            PreparedData pdata;
            pdata.imageTexture = (SDL_Texture*) TextureCache::get().book_get_texture(history.bookId);
            pdata.bookTitle = BookDatabase::get().query(history.bookId)->title;
            pdata.bookId = history.bookId;
            pdata.start = borrowTimeMap[history.bookId];
            pdata.endActual = history.time;
            pdata.endExpected = pdata.start.advanceDay(policy->getLoanDuration());
            pdata.isReturned = true;
            if (pdata.endActual > pdata.endExpected) {
                pdata.isOverdue = true;
                int overdueDays = pdata.endActual.dayDiff(pdata.endExpected);
                pdata.fine = policy->calculateFine(overdueDays);
            }
            incomingDataVec.push_back(pdata);
            borrowTimeMap.erase(history.bookId);
        } else if (history.action == "borrow" && borrowTimeMap.find(history.bookId) == borrowTimeMap.end()) {
            borrowTimeMap[history.bookId] = history.time;
        }
    }
    for(const auto& [bookId, startTime] : borrowTimeMap) {
        PreparedData pdata;
        pdata.imageTexture = (SDL_Texture*) TextureCache::get().book_get_texture(bookId);
        pdata.bookTitle = BookDatabase::get().query(bookId)->title;
        pdata.bookId = bookId;
        pdata.start = startTime;
        pdata.endExpected = pdata.start.advanceDay(policy->getLoanDuration());
        pdata.isReturned = false;
        if (Timestamp::now() > pdata.endExpected) {
            pdata.isOverdue = true;
            int overdueDays = Timestamp::now().dayDiff(pdata.endExpected);
            pdata.fine = policy->calculateFine(overdueDays);
        }
        incomingDataVec.push_back(pdata);
    }
}

void UIBorrowingStatusFrame::draw() {
        const float padding = 20.0f;

        if(incomingDataVec.size() > 0) {
            std::swap(dataVec, incomingDataVec);
            incomingDataVec.clear();
        }

        ImGui::PushID(this);
        for(int i=0; i < (int) dataVec.size(); i++) {
            auto& prepared = dataVec[i];
            ImGui::PushID(i);
            if(ImGui::BeginChild("BorrowingStatusCard", ImVec2(-1, 135), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));
                if (ImGui::BeginTable("##Alignment", 3)) {
                    ImGui::TableNextColumn();
                    ImGui::SetCursorPos(ImVec2(padding, padding));
                    ImGui::ImageNonStretch(prepared.imageTexture, 90, 90, appContext.darkMode);
                    ImGui::TableNextColumn();
                    ImGui::PushFont(appContext.boldfont);
                    ImGui::Text("%s", prepared.bookTitle.c_str());
                    ImGui::PopFont();
                    if(!prepared.isReturned) {
                        ImGui::Text("Borrow date: %s", prepared.start.toLangString().c_str());
                        ImGui::Text("Expected return date: %s", prepared.endExpected.toLangString().c_str());
                        if(prepared.isOverdue) {
                            ImGui::PushFont(appContext.boldfont);
                            ImGui::TextColored(appContext.getColorRed(), "OVERDUE");
                            ImGui::PopFont();
                            ImGui::TextColored(appContext.getColorRed(), "Fine: $%.2f", prepared.fine);
                        } else {
                            ImGui::PushFont(appContext.boldfont);
                            ImGui::TextColored(appContext.getColorYellow(), "BORROWING");
                            ImGui::PopFont();
                        }
                    } else {
                        ImGui::Text("Borrow date: %s", prepared.start.toLangString().c_str());
                        ImGui::Text("Return date: %s", prepared.endActual.toLangString().c_str());
                        if(prepared.isOverdue) {
                            ImGui::PushFont(appContext.boldfont);
                            ImGui::TextColored(appContext.getColorRed(), "RETURNED (OVERDUE)");
                            ImGui::PopFont();
                            ImGui::Text("Fine paid: $%.2f", prepared.fine);
                        } else {
                            ImGui::PushFont(appContext.boldfont);
                            ImGui::TextColored(appContext.getColorGreen(), "RETURNED");
                            ImGui::PopFont();
                        }
                    }
                    ImGui::TableNextColumn();
                    if(!prepared.isReturned) {
                        if(ImGui::Button("Return")) {
                            if(BorrowingService::get().returnBook(appContext.currentUser->getInternalId(), prepared.bookId)) {
                                refreshData();
                            }
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::PopStyleVar();
            }
            ImGui::EndChild();
            ImGui::PopID();
        }
        ImGui::PopID();
    } 
