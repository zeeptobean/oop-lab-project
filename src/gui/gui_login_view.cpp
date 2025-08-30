#include "gui/gui_login_view.hpp"

void UILoginView::draw() {
    if(isLoginOpen) {
        ImGui::OpenPopup("Login");
        isLoginOpen = false;
    }
    if (ImGui::BeginPopupModal("Login", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::SetNextWindowPos(ImVec2(appContext.windowSize.x / 2, appContext.windowSize.y / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::PushID("LoginPopup");
        ImGui::InputText("Email", &email, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::InputText("Password", &password, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Spacing();
        if(ImGui::Button("Login")) {
            appContext.currentUser = UserDatabase::get().login(email, password);
            if(appContext.currentUser) {
                ImGui::CloseCurrentPopup();
            } else {
                wrongAttempts++;
                password = "";
            }
        }
        if(wrongAttempts > 0) {
            ImGui::Text("Login failed. You have failed %u time(s).", wrongAttempts);
        }
        ImGui::PopID();
        ImGui::EndPopup();
    }
}