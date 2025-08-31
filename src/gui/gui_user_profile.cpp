#include "gui/gui_user_profile.hpp"

UIUserProfile::UIUserProfile(AppContext& context) : appContext(context), user(context.currentUser) {
    if(user) {
        imageTexture = IMG_LoadTexture(appContext.renderer, std::string("../" + user->image).c_str());
        uuser = dynamic_cast<UniversityUser*>(user);
        borrowingStatusFrame = std::make_unique<UIBorrowingStatusFrame>(appContext, user->getBorrowingPolicy());
        favoriteStatusFrame = std::make_unique<UIFavoriteStatusFrame>(appContext);
    }
}

void UIUserProfile::draw() {
        ImGui::PushID(this);
        ImGui::BeginChild("UserProfile", ImVec2(-1, -1));
        ImGui::ImageNonStretch(imageTexture, 100, 100);
        ImGui::SameLine();
        ImGui::BeginGroup();
        if(ImGui::CollapsingHeader("User information", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SeparatorText("Basic information");
            ImGui::Text("Name: %s", user->name.c_str());
            ImGui::Text("National ID: %s", user->nid.c_str());
            ImGui::Text("Date of Birth: %s", user->dob.toLangString().c_str());
            ImGui::Text("Address: %s", user->address.c_str());
            ImGui::Text("Phone: %s", user->phone.c_str());
            ImGui::Text("Email: %s", user->email.c_str());
        }
        if(uuser) {
            ImGui::SeparatorText("University specific information");
            ImGui::Text("%s ID: %s", user->type.c_str(), uuser->workId.c_str());
            ImGui::Text("Department: %s", uuser->department.c_str());
        } else {
            if(user->type == "premium") {
                ImGui::Text("User type: premium");
            } else {
                ImGui::Text("User type: regular");
            }
        }

        if(ImGui::CollapsingHeader("Favorite books listing")) {
            ImGui::SeparatorText("Book list");
            favoriteStatusFrame->draw();
        }

        if(ImGui::CollapsingHeader("Borrowing")) {
            ImGui::SeparatorText("Information");
            ImGui::Text("Loan duration: %d days", user->getBorrowingPolicy()->getLoanDuration());
            ImGui::Text("Max books allowed: %d", user->getBorrowingPolicy()->getMaxLoanAllowed());
            ImGui::Text("Currently borrowed: %d", BorrowingService::get().queryUserCurrentBorrowedCount(user->getInternalId()));
            if(user->isSuspended) {
                ImGui::TextColored(ImVec4(250, 0, 0, 1), "Account is suspended until %s", user->suspensionEnd.toLangString().c_str());
            }
            ImGui::SeparatorText("Transaction list");
            borrowingStatusFrame->draw();
        }
        if(ImGui::Button("Logout")) {
            appContext.requestLogout();
        }
        ImGui::EndGroup();
        ImGui::EndChild();
        ImGui::PopID();
    }