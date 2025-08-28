#include <bits/stdc++.h>

#include "imgui.h"
#include "imstb_truetype.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "gui_abstract.hpp"
#include "book.hpp"
#include "app_context.hpp"

ImVec2 windowSize;
SDL_Renderer *renderer;

SDL_Surface* loadImage(const std::string& filename) {
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if(!surface) {
        return nullptr;
    }
    return surface;
}

/*
SDL_Texture* getImageTexture(SDL_Surface *surface, float requestW, float requestH) {
    if(!surface) {
        return nullptr;
    }
    int original_width = surface->w;
    int original_height = surface->h;
    int new_width, new_height;
    float requestAspect = requestW / requestH;
    float imageAspect = (float) surface->w / (float) surface->h;
    if (requestAspect > imageAspect) {
        new_width = (int) requestW;
        new_height = (int)(requestW / imageAspect);
    } else {
        new_height = (int) requestH;
        new_width = (int) (requestH * imageAspect);
    }

    SDL_ScaleSurface(surface, new_width, new_height, SDL_SCALEMODE_NEAREST);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    return texture;
}
*/

namespace ImGui {
void TextEllipsisLines(const std::string& str, int maxLines = 2) {
    if (maxLines <= 0) return;

    const float lineHeight = ImGui::GetTextLineHeight();
    const float maxHeight = lineHeight * maxLines;
    const float availableWidth = ImGui::GetContentRegionAvail().x;

    // Measure full wrapped text
    ImVec2 fullTextSize = ImGui::CalcTextSize(str.c_str(), nullptr, true, availableWidth);

    if (fullTextSize.y <= maxHeight + 0.5f) {
        // Text fits fully
        ImGui::TextWrapped("%s", str.c_str());
        return;
    }

    // Needs truncation
    const std::string ellipsis = "...";
    int left = 0, right = (int)str.size();
    int bestFit = 0;

    while (left <= right) {
        int mid = (left + right) / 2;
        std::string candidate = str.substr(0, mid) + ellipsis;

        ImVec2 size = ImGui::CalcTextSize(candidate.c_str(), nullptr, true, availableWidth);

        if (size.y <= maxHeight + 0.5f) {
            bestFit = mid;
            left = mid + 1; // try longer
        } else {
            right = mid - 1; // try shorter
        }
    }

    std::string truncated = str.substr(0, bestFit) + ellipsis;
    ImGui::TextWrapped("%s", truncated.c_str());
}
}

ImFont* font, *boldfont;
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
    font = io.Fonts->AddFontFromFileTTF(regularFontPath, 16.0f);
    io.Fonts->AddFontFromFileTTF(jpFontPath, 0.0f, &fontConfigMerge);
    io.Fonts->AddFontFromFileTTF(emojiFontPath, 0.0f, &fontConfigMerge, emojiGlyphRange);
    boldfont = io.Fonts->AddFontFromFileTTF(boldFontPath, 16.0f);
    io.Fonts->AddFontFromFileTTF(jpBoldFontPath, 0.0f, &fontConfigMergeBold);
}

class TextureCache {
    public:
    static TextureCache& get() {
        static TextureCache instance;
        return instance;
    }

    void book_recache_all() {
        book_destroy_all();
        const std::vector<Book>& bookVec = BookDatabase::get().getAllBooks();
        for(auto& book : bookVec) {
            SDL_Surface* surface = loadImage("../" + book.imagePath);
            if(!surface) {
                bookTextureMap[book.internalId] = nullptr;
                continue;
            }
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
            bookTextureMap[book.internalId] = texture;
        }
    }

    SDL_Texture* book_get_texture(uint64_t bookId) {
        auto it = bookTextureMap.find(bookId);
        if(it != bookTextureMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    private:
    std::map<uint64_t, SDL_Texture*> bookTextureMap;

    void book_destroy_all() {
        for(auto& [id, texture] : bookTextureMap) {
            if(texture) {
                SDL_DestroyTexture(texture);
            }
        }
        bookTextureMap.clear();
    }

    TextureCache() {}
    ~TextureCache() {
        book_destroy_all();
    }
    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;
};

class UIBookView : public IUIAbstract {
    private:
    SDL_Texture *imageTexture;
    Book book;

    public:
    UIBookView(const Book& tbook) : book(tbook) {
        imageTexture = TextureCache::get().book_get_texture(book.internalId);
    }
    
    void draw() override {
        ImGui::PushID(book.internalId);
        ImGui::BeginGroup();
        ImGui::Image(imageTexture, ImVec2(480, 640));
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::PushFont(boldfont, 24.0f);
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetWindowWidth());
        ImGui::TextWrapped(book.title.c_str());
        ImGui::PopFont();
        ImGui::Text("ISBN: %llu", book.isbn);
        ImGui::Text("Author(s): %s", book.getAuthorsString().c_str());
        ImGui::Text("Publication year: %u", book.publicationYear);
        if(book.ageRating == 0) {
            ImGui::Text("Age rating: EVERYONE");
        } else {
            ImGui::Text("Age rating: %u+", book.ageRating);
        }
        if(ImGui::Button(std::string("Borrow").c_str())) {
            ImGui::OpenPopup("NotImplemented");
        }
        ImGui::PushFont(font, 14.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255)); //gray color
        ImGui::TextWrapped(book.getTagsString().c_str());
        ImGui::PopStyleColor();
        ImGui::PopFont();
        if (ImGui::BeginPopupModal("NotImplemented", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(250, 0, 0, 255)); //red color
            ImGui::Text("Not implemented");
            ImGui::PopStyleColor();
            if(ImGui::Button("Ok")) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        ImGui::EndGroup();
        ImGui::PopID();
    }
};

class UiBookCard {
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
    UiBookCard(AppContext& context) : appContext(context), book(nullptr) {
    }
    UiBookCard(AppContext& context, Book* tbook) : appContext(context), book(tbook) {
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
        ImGui::PushFont(boldfont, 20.0f);
        ImGui::TextEllipsisLines(book->title, 2);
        ImGui::PopFont();
        ImGui::Text("Author(s): %s", book->getAuthorsString().c_str());
        if(ImGui::Button("View Details")) {
            appContext.requestNewTab(std::make_unique<UIBookView>(*book), "Viewing " + book->title);
        }
        ImGui::EndChild();
        ImGui::EndChild();
        ImGui::PopID();
    }

    virtual ~UiBookCard() {}
};

class UiBookCatalogue : public IUIAbstract {
    private:
    AppContext& appContext;
    std::vector<std::unique_ptr<UiBookCard>> bookCards;
    public:
    UiBookCatalogue(AppContext& context, const std::vector<Book*>& tbooks) : appContext(context) {
        for(Book* book : tbooks) {
            bookCards.emplace_back(std::make_unique<UiBookCard>(context, book));
        }
    }
    void draw() override {
        for(auto& card : bookCards) {
            card->draw();
        }
    }
};

class UITabPage : public IUIAbstract {
    private:
    bool *state = nullptr;
    public:
    std::string title;
    std::unique_ptr<IUIAbstract> content;

    UITabPage(std::unique_ptr<IUIAbstract> tcontent, const std::string& ttitle, bool isClosable = true) 
    : content(std::move(tcontent)), title(ttitle) {
        if(isClosable) {
            state = new bool(true);
        }
    }

    void draw() override {
        ImGui::PushID(this);
        if(ImGui::BeginTabItem(title.c_str(), state)) {
            ImGui::BeginChild("TabPageContent", ImVec2(-1, -1));
            content->draw();
            ImGui::EndChild();  
            ImGui::EndTabItem();
        }
        ImGui::PopID();
    }

    bool isActive() const {
        return state ? *state : true;
    }
    virtual ~UITabPage() {
        delete state;
    };
};

class Application : public IUIAbstract {
    private:
    AppContext appContext;
    std::vector<std::unique_ptr<UITabPage>> uiPages; 

    public:
    Application(SDL_Renderer* mainRenderer) 
        : appContext(mainRenderer, [this](std::unique_ptr<IUIAbstract> content, const std::string& title) {
            this->uiPages.emplace_back(std::make_unique<UITabPage>(std::move(content), title));
        })      //init const appContext member
    {

        uiPages.push_back(std::make_unique<UITabPage>(std::make_unique<UiBookCatalogue>(appContext, BookDatabase::get().getLatestBooks(3)), "Latest Books", false));
    }

    void draw() override {
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
    renderer = SDL_CreateRenderer(window, nullptr);
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

    setupFont();
    // ImGui::StyleColorsLight();
    
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    BookDatabase::get().loadFile("../data/book/book.json");
    TextureCache::get().book_recache_all();
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
