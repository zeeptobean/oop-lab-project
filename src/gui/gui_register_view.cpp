#include "gui/gui_register_view.hpp"

void UIRegisterView::reset() {
    user = User();
    dobStr = passwordStr = passwordStr2 = "";
}

bool UIRegisterView::checkValidInput() {
    if(user.name.empty() || dobStr.empty() || user.address.empty() || user.phone.empty() || user.nid.empty() || user.email.empty() || passwordStr.empty() || passwordStr2.empty()) {
        return false;
    }
    if(passwordStr != passwordStr2) return false;
    for(auto& ch : user.phone) {
        if(!isdigit(ch)) {
            return false;
        }
    }
    return true;
}

void UIRegisterView::draw() {
    if(ImGui::BeginPopupModal("RegisterPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_Appearing);
        ImGui::Text("Name");
        ImGui::InputText("##Name", &user.name, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("Date of birth (input in format YYYY-MM-DDThh-mm)");
        ImGui::InputText("##DOB", &dobStr, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("Address");
        ImGui::InputText("##Address", &user.address, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("Phone number");
        ImGui::InputText("##Phone", &user.phone, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("National ID");
        ImGui::InputText("##NID", &user.nid, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Text("Email");
        ImGui::InputText("##Email", &user.email, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::Text("Password");
        ImGui::InputText("##Password", &passwordStr, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        if(passwordStr != passwordStr2) {
            ImGui::Text("Repeat password (currently do not match)");
        } else {
            ImGui::Text("Repeat password (currently match)");
        }
        ImGui::InputText("##Password2", &passwordStr2, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        ImGui::BeginDisabled(!checkValidInput());
        if(ImGui::Button("Register")) {
            user.dob = Timestamp(dobStr);
            UserDatabase::get().addUser(user, passwordStr);
            ImGui::CloseCurrentPopup();
            this->reset();
        }
        ImGui::EndDisabled();
        if(ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            this->reset();
        }
        ImGui::EndPopup();
    }
}