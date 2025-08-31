#include "gui/gui_login_view.hpp"

void UILoginView::resetRegister() {
    r_user = User();
    dobStr = passwordStr = passwordStr2 = "";
}

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
        if(ImGui::InputText("Email", &email, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft)) {
            forgotPassword = false;
        }
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
            forgotPassword = false;
        }
        if(ImGui::Button("Forgot password")) {
            forgotPassword = true;
        }
        if(ImGui::Button("Register")) {
            // isRegisterOpen = true;
            ImGui::OpenPopup("RegisterPopup");
            resetRegister();
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

    if(ImGui::BeginPopupModal("RegisterPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
        ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_Appearing);
        ImGui::PushID("RegisterPopupIDIDID");
        ImGui::Text("Name");
        ImGui::InputText("##Name", &r_user.name, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("Date of birth (input in format YYYY-MM-DDThh-mm)");
        ImGui::InputText("##DOB", &dobStr, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("Address");
        ImGui::InputText("##Address", &r_user.address, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("Phone number");
        ImGui::InputText("##Phone", &r_user.phone, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("National ID");
        ImGui::InputText("##NID", &r_user.nid, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Text("Email");
        ImGui::InputText("##Email", &r_user.email, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("Password");
        ImGui::InputText("##Password", &passwordStr, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("Repeat password");
        ImGui::InputText("##Password2", &passwordStr2, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        if(ImGui::Button("Register")) {
            if(passwordStr != passwordStr2) {
                ImGui::Text("Passwords do not match!");
            } else {
                r_user.dob = Timestamp(dobStr);
                UserDatabase::get().addUser(r_user, passwordStr);
                ImGui::CloseCurrentPopup();
                this->resetRegister();
            }
        }
        if(ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            this->resetRegister();
        }
        ImGui::PopID();
        ImGui::EndPopup();
    }
}