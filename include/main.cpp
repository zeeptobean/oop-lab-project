#include <bits/stdc++.h>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "gui_abstract.hpp"
#include "book.hpp"
#include "app_context.hpp"
#include "user.hpp"
#include "texture_cache.hpp"
#include "gui_carousel_view.hpp"
#include "custom_imgui_widget.hpp"
#include "gui_bookview.hpp"
#include "borrowing_service.hpp"

ImVec2 windowSize;

class UIBookCard {
    private:
    AppContext& appContext;
    SDL_Texture *texture = nullptr;
    Book *book;
    float last_image_width = 0.0f;
    float last_image_height = 0.0f;

    // void updateTexture(float new_width, float new_height) {
    //     if (texture) {
    //         SDL_DestroyTexture(texture);
    //     }
    //     // texture = getImageTexture(imageSurface, new_width, new_height);
    //     texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    // }

    public:
    UIBookCard(AppContext& context) : appContext(context), book(nullptr) {
    }
    UIBookCard(AppContext& context, Book* tbook) : appContext(context), book(tbook) {
        texture = TextureCache::get().book_get_texture(book->internalId);
    }
    void draw() {
        if(!book) return;
        ImGui::PushID(this); 
        ImGui::BeginChild("BookCard", ImVec2(-1, 285.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImVec2 available_space = ImGui::GetContentRegionAvail();
        float image_width = available_space.x * 0.25f;
        float image_height = image_width * (280.0f / 210.0f);
        // if (image_width != last_image_width || image_height != last_image_height) {
        //     updateTexture(image_width, image_height);
        //     last_image_width = image_width;
        //     last_image_height = image_height;
        // }
        ImGui::Image((void*)texture, ImVec2(image_width, image_height));
        ImGui::SameLine();
        ImGui::BeginChild("BookCardText", ImVec2(-1, -1), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PushFont(appContext.boldfont, 20.0f);
        ImGui::TextEllipsisLines(book->title, 2);
        ImGui::PopFont();
        ImGui::Text("Author(s): %s", book->getAuthorsString().c_str());
        if(ImGui::Button("View Details")) {
            appContext.requestNewTab(std::make_unique<UIBookView>(appContext, *book), "Viewing " + book->title);
        }
        ImGui::EndChild();
        ImGui::EndChild();
        ImGui::PopID();
    }

    virtual ~UIBookCard() {}
};

class UIBookCatalogue : public IUIAbstract {
    private:
    AppContext& appContext;
    std::vector<std::unique_ptr<UIBookCard>> bookCards;
    public:
    UIBookCatalogue(AppContext& context, const std::vector<Book*>& tbooks) : appContext(context) {
        for(Book* book : tbooks) {
            bookCards.emplace_back(std::make_unique<UIBookCard>(context, book));
        }
    }
    void draw() override {
        for(auto& card : bookCards) {
            card->draw();
        }
    }
};

class UIUserProfile : public IUIAbstract {
    private:
    User *user;
    UniversityUser *uuser = nullptr;
    AppContext& appContext;
    SDL_Texture *imageTexture = nullptr;

    public:
    UIUserProfile(AppContext& context, User* tuser) : appContext(context), user(tuser) {
        if(user) {
            SDL_Surface* surface = loadImage("../" + user->image);
            if(surface) {
                imageTexture = SDL_CreateTextureFromSurface(appContext.renderer, surface);
                SDL_DestroySurface(surface);
            } else {
                surface = loadImage("../data/user/default.png");
                if(surface) {
                    imageTexture = SDL_CreateTextureFromSurface(appContext.renderer, surface);
                    SDL_DestroySurface(surface);
                }
            }

            uuser = dynamic_cast<UniversityUser*>(user);
        }
    }

    void draw() override {
        ImGui::PushID(this);
        ImGui::BeginChild("UserProfile", ImVec2(-1, -1));
        ImGui::Image((void*)imageTexture, ImVec2(100, 100));
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
            std::string userTypeStr;
            switch(uuser->userType) {
                case UniversityUserType::UNDERGRADUATE:
                userTypeStr = "Undergraduate";
                break;
                case UniversityUserType::POSTGRADUATE:
                userTypeStr = "Postgraduate";
                break;
                case UniversityUserType::TEACHER:
                userTypeStr = "Teacher";
                break;
                case UniversityUserType::STAFF:
                userTypeStr = "Staff";
                break;
                default:
                    userTypeStr = "Unknown";
                    break;
                }
            ImGui::Text("%d ID: %s", userTypeStr, uuser->workId.c_str());
            ImGui::Text("Department: %s", uuser->department.c_str());
        }

        if(ImGui::CollapsingHeader("Borrowing", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SeparatorText("Information");

        }
        if(ImGui::Button("Logout")) {
            appContext.requestLogout();
        }
        ImGui::EndGroup();
        ImGui::EndChild();
        ImGui::PopID();
    }
};

class UIComposedView : public IUIAbstract {
    private:
    std::vector<std::unique_ptr<IUIAbstract>> views;
    AppContext& appContext;

    void defaultDrawFunc() {
        for(auto& view : views) {
            view->draw();
        }
    }

    public:
    std::function<void(void)> customDrawFunc = nullptr;
    UIComposedView(AppContext& context) : appContext(context), customDrawFunc(std::bind(&UIComposedView::defaultDrawFunc, this)) {}
    void addView(std::unique_ptr<IUIAbstract> view) {
        views.emplace_back(std::move(view));
    }
    void draw() override {
        customDrawFunc();
    }
};

class UITabPage : public IUIAbstract {
    private:
    bool closable = true;
    bool active = true;
    bool *ptr = nullptr;
    std::string title;
    public:
    std::unique_ptr<IUIAbstract> content;

    UITabPage(std::unique_ptr<IUIAbstract> tcontent, const std::string& ttitle, bool isClosable = true) 
    : content(std::move(tcontent)), title(ttitle), closable(isClosable) {
        if(closable) ptr = &active;
    }

    UITabPage(UITabPage&& other)
        : content(std::move(other.content)),
          title(std::move(other.title)),
          closable(other.closable),
          active(other.active)
    {
        other.content = nullptr;
    }

    // Move Assignment Operator
    UITabPage& operator=(UITabPage&& other) noexcept {
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

    UITabPage(const UITabPage&) = delete;
    UITabPage& operator=(const UITabPage&) = delete;

    void draw() override {
        ImGui::PushID(this);
        if(ImGui::BeginTabItem(title.c_str(), ptr)) {
            ImGui::BeginChild("TabPageContent", ImVec2(-1, -1) , false, ImGuiWindowFlags_HorizontalScrollbar);
            content->draw();
            ImGui::EndChild();  
            ImGui::EndTabItem();
        }
        ImGui::PopID();
    }

    bool isActive() const {
        return closable ? active : true;
    }
    virtual ~UITabPage() {};
};

class UILoginView : public IUIAbstract {
    private:
    bool isLoginOpen = true;
    AppContext& appContext;
    std::string email, password, accessToken;
    unsigned int wrongAttempts = 0;
    public:

    UILoginView() = default;
    UILoginView(AppContext& ctx) : appContext(ctx) {  }

    std::string getAccessToken() const {
        return accessToken;
    }

    void reset() {
        email = "";
        password = "";
        wrongAttempts = 0;
        isLoginOpen = true;
    }

    void draw() override {
        if(isLoginOpen) {
            ImGui::OpenPopup("Login");
            isLoginOpen = false;
        }
        if (ImGui::BeginPopupModal("Login", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
            ImGui::SetNextWindowPos(ImVec2(windowSize.x / 2, windowSize.y / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
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
};

class Application : public IUIAbstract {
    private:
    AppContext appContext;
    std::vector<std::unique_ptr<UITabPage>> uiPages, incomingUiPages; 
    std::function<void(void)> currentDrawContext;

    UILoginView loginView;
    std::string accessToken = "";

    void setupFont() {
        const char* regularFontPath = "../data/font/bank/Noto_Sans_Display/static/NotoSansDisplay-Medium.ttf";
        const char* boldFontPath = "../data/font/bank/Noto_Sans_Display/static/NotoSansDisplay-Bold.ttf";
        const char* jpFontPath = "../data/font/bank/Noto_Sans_JP/static/NotoSansJP-Medium.ttf";
        const char* jpBoldFontPath = "../data/font/bank/Noto_Sans_JP/static/NotoSansJP-Bold.ttf";
        const char* emojiFontPath = "../data/font/bank/Noto_Emoji/static/NotoEmoji-Medium.ttf";

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
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
        ImGui::Begin("Hello, world!", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove);
        ImGui::Text("This is some useful text.");
        if(ImGui::BeginTabBar("MyTabs", ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyMask_ | ImGuiTabBarFlags_DrawSelectedOverline)) {
            for(auto& page : uiPages) {
                page->draw();
            }
        }
        ImGui::EndTabBar();
        ImGui::End();

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
            addDefaultPage();
            currentDrawContext = std::bind(&Application::drawBrowser, this);
        }
    }

    void addDefaultPage() {
        uiPages.push_back(std::make_unique<UITabPage>(std::make_unique<UICarouselView>(appContext, "../data/book/carousel.json"), "Homepage", false));
        uiPages.push_back(std::make_unique<UITabPage>(std::make_unique<UIUserProfile>(appContext, appContext.currentUser), "Profile", false));
    }

    public:
    Application(SDL_Renderer* mainRenderer) 
        : appContext(mainRenderer, 
            [this](std::unique_ptr<IUIAbstract> content, const std::string& title) {
                this->incomingUiPages.emplace_back(std::make_unique<UITabPage>(std::move(content), title));
            },
            [this]() {
                this->appContext.currentUser = nullptr;
                this->loginView.reset();
                this->uiPages.clear();
                this->currentDrawContext = std::bind(&Application::drawLogin, this);
            }
        )   //init const appContext member
        , loginView(appContext)
    {
        setupFont();
        if(SDL_GetSystemTheme() == SDL_SYSTEM_THEME_DARK) {     //require sdl 3.2.0
            appContext.darkMode = true;
            ImGui::StyleColorsDark();
        } else {
            appContext.darkMode = false;
            ImGui::StyleColorsLight();
        }
        currentDrawContext = std::bind(&Application::drawLogin, this);
        
    }

    void draw() override {
        currentDrawContext();
    }
    ~Application() override = default;
};


int main(int, char**) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    windowSize = ImVec2(1280 * main_scale, 720 * main_scale);

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
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;

    // ImGui::StyleColorsLight();
    
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    UserDatabase::get().loadFile("../data/user/user.json");
    BookDatabase::get().loadFile("../data/book/book.json");
    BorrowingService::get().loadBookStock("../data/book/book_stock.json");
    BorrowingService::get().loadBorrowingHistory("../data/book/borrowing_history.json");
    TextureCache::get().init(renderer);
    Application app (renderer);

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
    return 0;
}
