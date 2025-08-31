#include <bits/stdc++.h>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "core/book.hpp"
#include "core/book_database.hpp"

#include "gui/gui_login_view.hpp"
#include "gui/gui_tabpage.hpp"
#include "gui/gui_abstract.hpp"
#include "gui/gui_carousel_view.hpp"
#include "gui/gui_bookview.hpp"
#include "gui/gui_user_profile.hpp"
#include "gui/gui_search.hpp"
#include "gui/gui_composed_view.hpp"

#include "app_context.hpp"
#include "texture_cache.hpp"
#include "event_dispatcher.hpp"
#include "virtual_calendar.hpp"

class Application : public IUIAbstract {
    private:
    AppContext appContext;
    std::vector<std::unique_ptr<UITabPage>> uiPages, incomingUiPages; 
    std::function<void(void)> currentDrawContext;

    

    UILoginView loginView;
    std::string accessToken = "";

    bool doLogout = false;

    void setupHomepage() {
        std::unique_ptr<UIComposedView> frontPageView (new UIComposedView(appContext));
        std::ifstream infile ("../data/carousel.json");
        nlohmann::json masterCarouselConfig;
        try {
            infile >> masterCarouselConfig;

            for (auto& [department, lists] : masterCarouselConfig.items()) {
                if(department == "regular" || appContext.currentUser->checkDepartmentRestriction({department})) {
                    for (const auto& list_item : lists) {
                        frontPageView->addView(std::make_unique<UICarouselView>(appContext, list_item));
                    }
                }
            }

            infile.close();
        } catch(...) {
            assert(false && "homepage error");
        }
        uiPages.push_back(std::make_unique<UITabPage>(std::move(frontPageView), "Home", false));
    }

    void setupFont() {
        const char* regularFontPath = "../data/font/NotoSansDisplay-Medium.ttf";
        const char* boldFontPath = "../data/font/NotoSansDisplay-Bold.ttf";
        const char* jpFontPath = "../data/font/NotoSansJP-Medium.ttf";
        const char* jpBoldFontPath = "../data/font/NotoSansJP-Bold.ttf";
        const char* emojiFontPath = "../data/font/NotoEmoji-Medium.ttf";

        //manual emoji range as imgui 1.92+ can't autodetect properly
        const ImWchar emojiGlyphRange[9] = {
            0x00a9, 0x00ae,     //copyright & registered trademark
            0x203c, 0x329f,     //basic emoji
            0x1f000, 0x1faff,   //emoji
            0x1F3FB, 0x1F3FF,   //emoji modifier
            0
        };

        ImFontConfig fontConfigMerge;
        fontConfigMerge.MergeMode = true;
        fontConfigMerge.PixelSnapH = true;
        fontConfigMerge.OversampleH = 2;
        ImFontConfig fontConfigMergeBold;
        fontConfigMergeBold.MergeMode = true;
        fontConfigMergeBold.PixelSnapH = true;
        fontConfigMergeBold.OversampleH = 2;

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        appContext.font = io.Fonts->AddFontFromFileTTF(regularFontPath, 16.0f);
        io.Fonts->AddFontFromFileTTF(jpFontPath, 0.0f, &fontConfigMerge);
        io.Fonts->AddFontFromFileTTF(emojiFontPath, 0.0f, &fontConfigMerge, emojiGlyphRange);
        appContext.boldfont = io.Fonts->AddFontFromFileTTF(boldFontPath, 16.0f);
        io.Fonts->AddFontFromFileTTF(jpBoldFontPath, 0.0f, &fontConfigMergeBold);
    }

    void drawBrowser() {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(appContext.windowSize, ImGuiCond_Always);
        ImGui::Begin("Hello, world!", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove);
        drawMenuBar();
        if(ImGui::BeginTabBar("MyTabs", ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyMask_ | ImGuiTabBarFlags_DrawSelectedOverline)) {
            for(size_t i = 0; i < uiPages.size(); ++i) {
                ImGui::PushID(i);
                uiPages[i]->draw();
                ImGui::PopID();
            }
        }
        ImGui::EndTabBar();
        ImGui::End();

        if(doLogout) {
            doLogout = false;
            appContext.currentUser = nullptr;
            uiPages.clear();
            loginView.resetLogin();
            currentDrawContext = std::bind(&Application::drawLogin, this);
            return;
        }

        for(auto it = uiPages.begin(); it != uiPages.end(); ) {
            if(!(*it)->isActive()) {
                it = uiPages.erase(it);
            } else {
                it++;
            }
        }

        for(auto& newPage : incomingUiPages) {
            uiPages.push_back(std::move(newPage));
        }
        incomingUiPages.clear();
    }

    void drawLogin() {
        loginView.draw();
        if(appContext.currentUser) {
            addDefaultTabs();
            currentDrawContext = std::bind(&Application::drawBrowser, this);
        }
    }

    void drawMenuBar() {
        ImGui::BeginMenuBar();
        if(ImGui::BeginMenu("Menu")) {
            if(ImGui::BeginMenu("Theme")) {
                if(ImGui::MenuItem("Light", nullptr, !appContext.darkMode)) {
                    appContext.darkMode = false;
                    ImGui::StyleColorsLight();
                }
                if(ImGui::MenuItem("Dark", nullptr, appContext.darkMode)) {
                    appContext.darkMode = true;
                    ImGui::StyleColorsDark();
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Logout")) {
                appContext.requestLogout();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Virtual calendar")) {
            ImGui::MenuItem("Current virtual date: ", nullptr, false, false);
            ImGui::MenuItem(Timestamp::now().toLangString().c_str(), nullptr, false, false);
            ImGui::Separator();
            if(ImGui::MenuItem("Advance 1 day")) {
                VirtualCalendar::get().advanceDay(1);
            }
            if(ImGui::MenuItem("Advance 7 days")) {
                VirtualCalendar::get().advanceDay(7);
            }
            if(ImGui::MenuItem("Set current time")) {
                VirtualCalendar::get().setDate(Timestamp());
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    void addDefaultTabs() {
        setupHomepage();
        uiPages.push_back(std::make_unique<UITabPage>(std::make_unique<UIUserProfile>(appContext), "Profile", false));
        uiPages.push_back(std::make_unique<UITabPage>(std::make_unique<UISearch>(appContext), "Book Search", false));
    }

    public:
    Application(SDL_Renderer* mainRenderer, const ImVec2& windowSize) 
        : appContext(mainRenderer, windowSize,
            [this](std::unique_ptr<IUIAbstract> content, const std::string& title) {
                this->incomingUiPages.emplace_back(std::make_unique<UITabPage>(std::move(content), title));
            },
            [this]() {
                this->doLogout = true;
            }
        )   //init const appContext member
        , loginView(appContext)
    {
        setupFont();
        loadSetting();
        currentDrawContext = std::bind(&Application::drawLogin, this);
        
    }

    void loadSetting() {
        std::ifstream infile("../data/setting.json");
        nlohmann::json j;
        try {
            infile >> j;

            if(j.contains("dark_mode")) {
                appContext.darkMode = j["dark_mode"].get<bool>();
                if(appContext.darkMode) {
                    ImGui::StyleColorsDark();
                } else {
                    ImGui::StyleColorsLight();
                }
            } else {
                if(SDL_GetSystemTheme() == SDL_SYSTEM_THEME_DARK) {     //require sdl 3.2.0
                    appContext.darkMode = true;
                    ImGui::StyleColorsDark();
                } else {
                    appContext.darkMode = false;
                    ImGui::StyleColorsLight();
                }
            }

            if(j.contains("virtual_date")) {
                //if Timestamp constructor fails, it will automatically set to current date
                VirtualCalendar::get().setDate(Timestamp(j["virtual_date"].get<std::string>()));
            } else {
                //by default, VirtualCalendar use default Timestamp constructor which is current date
            }
            infile.close();
        } catch(...) {
            return;
        }
    }

    void saveSetting() {
        nlohmann::json j;
        j["dark_mode"] = appContext.darkMode;
        j["virtual_date"] = VirtualCalendar::get().currentTime.toString();

        std::ofstream outfile("../data/setting.json");
        try {
            outfile << j.dump(4);
            outfile.close();
        } catch(...) {
            return;
        }
    }

    void draw() override {
        currentDrawContext();
    }

    void updateWindowSize(const ImVec2& newSize) {
        appContext.windowSize = newSize;
    }

    ~Application() {
        saveSetting();
    };
};


int main(int, char**) {
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    ImVec2 windowSize = ImVec2(1280 * main_scale, 720 * main_scale);

    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window = SDL_CreateWindow("oop-lab-project", (int)windowSize.x, (int)windowSize.y, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return -1;
    }

    // IMG_Init()
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    UserDatabase::get().loadFile("../data/user/user.json");
    BookDatabase::get().loadFile("../data/book/book.json");
    BorrowingService::get().loadBookStock("../data/book/book_stock.json");
    BorrowingService::get().loadBorrowingHistory("../data/borrowing_history.json");
    TextureCache::get().init(renderer);
    Application app (renderer, windowSize);

    bool mainLoopDone = false;
    while (!mainLoopDone) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                mainLoopDone = true;
                continue;
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)) {
                mainLoopDone = true;
                continue;
            }
            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                int new_width = event.window.data1;
                int new_height = event.window.data2;
                windowSize.x = (float)new_width;
                windowSize.y = (float)new_height;
                app.updateWindowSize(windowSize);
                SDL_SetRenderLogicalPresentation(renderer, new_width, new_height, SDL_LOGICAL_PRESENTATION_STRETCH);
            }
        }

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        app.draw();

        ImGui::Render();
        SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0.5f, 1.0f);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    //write back data at the end for a successful exit, avoid data corruption
    BorrowingService::get().writeBorrowingHistory();
    BorrowingService::get().writeBookStock();
    UserDatabase::get().writeFile();

    return 0;
}
