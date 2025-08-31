#include "gui/gui_login_view.hpp"

void UILoginView::resetLogin() {
    email = "";
    password = "";
    wrongAttempts = 0;
    isLoginOpen = true;
}

void UILoginView::draw() {
    if(isLoginOpen) {
        ImGui::OpenPopup("Login");
        isLoginOpen = false;
    }
    if (ImGui::BeginPopupModal("Login", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::SetNextWindowPos(ImVec2(appContext.windowSize.x / 2, appContext.windowSize.y / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::PushID("LoginPopup");

        registerView->draw();

        if(ImGui::InputText("Email", &email, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft)) {
            forgotPassword = false;
        }
        ImGui::InputText("Password", &password, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Spacing();
        if(ImGui::Button("Login")) {
            appContext.currentUser = UserDatabase::get().login(email, password);
            if(appContext.currentUser && appContext.currentUser->getInternalId() != 0) {
                ImGui::CloseCurrentPopup();
            } else {
                wrongAttempts++;
                password = "";
            }
            forgotPassword = false;
        }
        if(ImGui::Button("Forgot password")) {
            forgotPassword = true;
        }
        if(ImGui::Button("Register")) {
            ImGui::OpenPopup("RegisterPopup");
        }
        if(forgotPassword) {
            auto hashstr = UserDatabase::get().queryUserPasswordHash(email);
            if(hashstr != "") {
                ImGui::TextColored(appContext.getColorYellow(), "Your password's SHA-256 hash is: %s. Good luck finding!", hashstr.c_str());
            } else {
                ImGui::TextColored(appContext.getColorRed(), "No account with given email.");
            }
        }
        if(wrongAttempts > 0) {
            ImGui::Text("Login failed. You have failed %u time(s).", wrongAttempts);
        }

        ImGui::PopID();
        ImGui::EndPopup();
    }
}