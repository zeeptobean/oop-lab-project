#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_abstract.hpp"
#include "book.hpp"
#include "app_context.hpp"
#include "texture_cache.hpp"
#include "borrowing_history.hpp"
#include "borrowing_service.hpp"

class UIBookView : public IUIAbstract {
    private:
    SDL_Texture *imageTexture;
    Book book;
    AppContext& appContext;
    bool ageRestrictionActivate = false;
    bool borrowConfirmationTick = false;

    bool checkEligibleAge() {
        if (!appContext.currentUser) return false;
        return appContext.currentUser->dob.yearDiff(Timestamp()) >= book.ageRating;
    }

    public:
    UIBookView(AppContext& ctx, const Book& tbook) : book(tbook), appContext(ctx) {
        imageTexture = TextureCache::get().book_get_texture(book.internalId);
        ageRestrictionActivate = !checkEligibleAge();
    }

    void drawImpl() {
        ImGui::PushID(this);
        ImGui::BeginGroup();
        ImGui::Image(imageTexture, ImVec2(480, 640));
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::PushFont(appContext.boldfont, 24.0f);
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetWindowWidth());
        ImGui::TextWrapped(book.title.c_str());
        ImGui::PopFont();
        ImGui::Text("ISBN: %llu", book.isbn);
        ImGui::Text("Author(s): %s", book.getAuthorsString().c_str());
        ImGui::Text("Publication year: %u", book.publicationYear);
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
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 250, 0, 255));
            ImGui::Text("You are borrowing this book");
            ImGui::PopStyleColor();
        } else if(appContext.currentUser->isSuspended) {
            ImGui::BeginDisabled();
            ImGui::Button("Borrow");
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(250, 0, 0, 255));
            ImGui::Text("Your account is suspended");
            ImGui::PopStyleColor();  
        } else if(BorrowingService::get().queryBookCount(book.internalId) <= 0) {
            ImGui::BeginDisabled();
            ImGui::Button("Borrow");
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(250, 0, 0, 255)); 
            ImGui::Text("Not available");
            ImGui::PopStyleColor();
        } else {
            if(ImGui::Button("Borrow")) {
                ImGui::OpenPopup("Borrowing book confirmation");
            }
        }

        ImGui::PushFont(appContext.font, 14.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255)); //gray color
        ImGui::TextWrapped(book.getTagsString().c_str());
        ImGui::PopStyleColor();
        ImGui::PopFont();
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
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(250, 0, 0, 255));
                    ImGui::Text("Failed to borrow book. Please try again later.");
                    ImGui::PopStyleColor();
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
    
    void draw() override {
        if(ageRestrictionActivate) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(250, 0, 0, 255)); //red color
            ImGui::PushFont(appContext.boldfont, 18.0f);
            ImGui::Text("You are not eligible to view this book due to age restriction.");
            ImGui::PopFont();
            ImGui::PopStyleColor();
        } else {
            drawImpl();
        }
    }
};