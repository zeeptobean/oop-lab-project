#include "gui/gui_register_view.hpp"

void UIRegisterView::reset() {
    user = User();
    dobStr = passwordStr = passwordStr2 = "";
}

void UIRegisterView::draw() {
    ImGui::OpenPopup("Register");
    if(ImGui::BeginPopupModal("Register", &isOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
        ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_Appearing);
        ImGui::PushID("RegisterPopup");
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
        ImGui::Text("Repeat password");
        ImGui::InputText("##Password2", &passwordStr2, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_ElideLeft);
        if(ImGui::Button("Register")) {
            if(passwordStr != passwordStr2) {
                ImGui::Text("Passwords do not match!");
            } else {
                user.dob = Timestamp(dobStr);
                UserDatabase::get().addUser(user, passwordStr);
                ImGui::CloseCurrentPopup();
                this->reset();
            }
        }
        ImGui::PopID();
        ImGui::EndPopup();
    }
}