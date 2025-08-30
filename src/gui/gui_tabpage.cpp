#include "gui/gui_tabpage.hpp"

UITabPage::UITabPage(std::unique_ptr<IUIAbstract> tcontent, const std::string& ttitle, bool isClosable) 
    : content(std::move(tcontent)), title(ttitle), closable(isClosable) {
        if(closable) ptr = &active;
        unique_title = title + "##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    }

UITabPage::UITabPage(UITabPage&& other)
        : content(std::move(other.content)),
          title(std::move(other.title)),
          closable(other.closable),
          active(other.active),
          unique_title(std::move(other.unique_title))
    {
        other.content = nullptr;
    }

UITabPage& UITabPage::operator=(UITabPage&& other) noexcept {
    if (this != &other) {
        content = std::move(other.content);
        title = std::move(other.title);
        closable = other.closable;
        active = other.active;

        // Steal the pointer from the other object
        other.content = nullptr;
    }
    return *this;
}

void UITabPage::draw() {
    if(ImGui::BeginTabItem(unique_title.c_str(), ptr)) {
        ImGui::PushID(this);
        ImGui::BeginChild("TabPageContent", ImVec2(-1, -1) , false, ImGuiWindowFlags_HorizontalScrollbar);
        content->draw();
        ImGui::EndChild();  
        ImGui::EndTabItem();
        ImGui::PopID();
    }
}

bool UITabPage::isActive() const {
    return closable ? active : true;
}