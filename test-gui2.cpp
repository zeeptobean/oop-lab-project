#include "imgui.h"
#include "imgui-SFML.h"
#include "helper.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <algorithm>

sf::RenderWindow window;

void testbed()
{
    ImGui::Begin("My Application Window");

    ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_Reorderable | 
                                ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyMask_ | 
                                ImGuiTabBarFlags_DrawSelectedOverline;
    if (ImGui::BeginTabBar("MyTabs", tabFlags)) // Unique ID for the tab bar
    {
        // --- Tab 1: Settings ---
        if (ImGui::BeginTabItem("Settings")) // Label for the tab
        {
            // Content for the "Settings" tab
            ImGui::Text("Adjust your application settings here.");
            if (ImGui::Button("Delete..")) ImGui::OpenPopup("Delete?");

            
            if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!");
                ImGui::Separator();

                //static int unused_i = 0;
                //ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

                static bool dont_ask_me_next_time = false;
                // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
                // ImGui::PopStyleVar();

                if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }
            

            ImGui::EndTabItem(); // IMPORTANT: End the tab item
        }

        std::vector<std::string> myLogEntries = {"lorem", "ipsum", "hent"};

        // --- Tab 2: Logs ---
        if (ImGui::BeginTabItem("Logs"))
        {
            // Content for the "Logs" tab
            ImGui::Text("Recent activity log:");
            ImGui::BeginChild("LogContent", ImVec2(0, 150), true); // A child window for scrollable log
            for (const auto& logEntry : myLogEntries)
            {
                ImGui::TextUnformatted(logEntry.c_str());
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        // --- Tab 3: About (with a close button) ---
        static bool showAboutTab = true; // State for whether this tab should be shown
        if (ImGui::BeginTabItem("About", &showAboutTab)) // Pass p_open to enable close button
        {
            ImGui::Text("This is a demo application.");
            ImGui::Text("Version: 1.0");
            ImGui::EndTabItem();
        }

        ImGui::BeginTabItem("tab1");
        ImGui::BeginTabItem("tab2");
        ImGui::BeginTabItem("tab3");
        ImGui::BeginTabItem("tab4");
        ImGui::BeginTabItem("tab5");
        ImGui::BeginTabItem("tab6");
        ImGui::BeginTabItem("tab7");
        ImGui::BeginTabItem("tab8");

        ImGui::EndTabBar(); // IMPORTANT: End the tab bar
    }

    ImGui::End();
}

void testbed2() {
    
}

int main() {
    window.create(sf::VideoMode(800, 600), "ImGui SFML GUI");
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);
    ImGui::StyleColorsLight();

    ImGuiIO& io = ImGui::GetIO();
    UNREFERENCED_PARAMETER(io);
    io.IniFilename = nullptr;

    sf::Clock deltaClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::SetNextWindowSize(ImVec2(400, 400));
        ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::BeginChild("Child Window 1", ImVec2(200, 200), true); // true for border
            ImGui::Text("This is Child Window 1");
            ImGui::EndChild();

            ImGui::SameLine(); // Place the next child window on the same line

            // Create another child window
            ImGui::BeginChild("Child Window 2", ImVec2(200, 200), true);
            ImGui::Text("This is Child Window 2");
            ImGui::EndChild();
        ImGui::End();

        ImGui::Begin("Main Window2", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::End();

        ImGui::Begin("Main Window3", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::End();

        testbed();

        window.clear(sf::Color::White); // Clear background to black
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}