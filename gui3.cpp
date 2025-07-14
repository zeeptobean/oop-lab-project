#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include "imgui-SFML.h"
#include "imgui.h"
#include "nlohmann/json.hpp"
#include <bits/stdc++.h>
using namespace std;
using json = nlohmann::json;

#define UNREFERENCED_PARAMETER(x) ((x) = (x))

sf::RenderWindow window;
ImVec2 windowSize(800, 600);

ImFont *fontExtraLarge, *fontLarge, *fontMedium, *fontSmall, *fontExtraSmall; 
ImFont *fontLargeBold, *fontMediumBold, *fontSmallBold;

const ImWchar regularGlyphRange[37] = {
    // Latin Scripts
    0x0020, 0x007F,   // Basic Latin
    0x0080, 0x00FF,   // Latin-1 Supplement
    0x0100, 0x017F,   // Latin Extended-A
    0x0180, 0x024F,   // Latin Extended-B

    // European Scripts
    0x0250, 0x02AF,   // IPA Extensions
    0x0300, 0x036F,   // Combining Diacritical Marks
    0x0370, 0x03FF,   // Greek and Coptic
    0x0400, 0x04FF,   // Cyrillic
    0x0500, 0x052F,   // Cyrillic Supplement
    0x1E00, 0x1EFF,   // Latin Extended Additional

    // Symbols and Punctuation
    0x2000, 0x206F,   // General Punctuation
    0x2070, 0x209F,   // Superscripts and Subscripts
    0x20A0, 0x20CF,   // Currency Symbols
    0x2190, 0x21FF,   // Arrows
    0x2200, 0x22FF,   // Mathematical Operators
    0x2300, 0x23FF,   // Miscellaneous Technical
    0x2500, 0x257F,   // Box Drawing
    0x2580, 0x259F,   // Block Elements
    0
};

const ImWchar emojiGlyphRange[9] = {
    0x00a9, 0x00ae,     //copyright & registered trademark
    0x203c, 0x329f,     //basic emoji
    0x1f000, 0x1faff,   //emoji
    0x1F3FB, 0x1F3FF,   //emoji modifier
    0
};

std::wstring stringToWideString(const std::string& tstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    std::wstring twstr = converter.from_bytes(tstr);
    return twstr;
}

std::string wideStringToString(const std::wstring& twstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    std::string tstr = converter.to_bytes(twstr);
    return tstr;
}

bool LoadFile(const std::string& filename, char*& mem, int& memsize) {
    std::ifstream infile(filename, std::ios::in | std::ios::binary);
    if(!infile) return false;
    infile.seekg(0, std::ios::end);
    memsize = (int) infile.tellg();
    infile.seekg(0, std::ios::beg);
    mem = new char[memsize];
    if(!infile.read(mem, memsize)) {
        infile.close();
        return false;
    }
    infile.close();
    return true;
}

//own extension
namespace ImGui {
    void Text2(const std::string& str) {
        ImGui::TextUnformatted(str.begin().base(), str.end().base());
    }

    void TextWide2(const std::wstring& wstr) {
        std::string str = wideStringToString(wstr);
        ImGui::TextUnformatted(str.begin().base(), str.end().base());
    }

    void TextEllipsisTwoLines(const std::string& str) {
        ImGuiIO& io = ImGui::GetIO();
        UNREFERENCED_PARAMETER(io);
        ImFont* font = ImGui::GetFont();
        int availableWidth = (int) ImGui::GetContentRegionAvail().x;

        float lineHeight = font->FontSize; // Or font->LineSpacing
        float twoLineHeight = lineHeight * 2.0f;

        ImVec2 fullTextSizeWrapped = ImGui::CalcTextSize(str.begin().base(), str.end().base(), true, availableWidth);

        if (fullTextSizeWrapped.y <= twoLineHeight + 1.0f) {
            ImGui::TextWrapped(str.c_str());
        } else {
            // Needs truncation
            std::string truncatedTitle = str;
            std::string ellipsis = "...";

            while (truncatedTitle.length() > 0) {
                std::string tempText = truncatedTitle + ellipsis;
                ImVec2 currentSize = ImGui::CalcTextSize(str.begin().base(), str.end().base(), true, availableWidth);

                if (currentSize.y <= twoLineHeight + 1.0f) {
                    break;
                }
                truncatedTitle.pop_back();
            }

            if (truncatedTitle.empty() && str.length() > 0) {
                ImGui::TextWrapped("..."); // Or a minimal truncated part
            } else {
                ImGui::TextWrapped("%s%s", truncatedTitle.c_str(), ellipsis.c_str());
            }
        }
    }

    void TextWideEllipsisTwoLines(const std::wstring& wstr) {
        TextEllipsisTwoLines(wideStringToString(wstr));
    }
}
namespace nlohmann {
    template <>
    struct adl_serializer<std::wstring> {
        static void to_json(json& j, const std::wstring& wstr) {
            j = wideStringToString(wstr);
        }

        static void from_json(const json& j, std::wstring& wstr) {
            wstr = stringToWideString(j.get<std::string>());
        }
    };
}

class GlobalMemory {
    public:
    char *regularFontMem, *japaneseFontMem, *emojiFontMem;

    static GlobalMemory& get() {
        static GlobalMemory instance;
        return instance;
    }
    GlobalMemory(const GlobalMemory&) = delete;
    GlobalMemory& operator=(const GlobalMemory&) = delete;

    private:
    GlobalMemory() = default;
    ~GlobalMemory() {
        delete[] regularFontMem; 
        delete[] japaneseFontMem;
        delete[] emojiFontMem;
    }
};

class Book {
    private:
    uint64_t id;
    std::string imagePath;

    std::wstring title;
    std::vector<std::wstring> authors;
    uint32_t publicationYear;
    uint32_t ageRating;
    uint64_t isbn;
    std::vector<std::wstring> tags;

    public:
    Book() {}
    // Book(const Book& tbook) {}

    friend class BookDatabase;

    std::wstring getTitle() const { return title; }
    std::wstring getAuthorsString() const { 
        std::wstring ret;
        for(size_t i=0; i < authors.size()-1; i++) ret = ret + authors[i] + L", ";
        ret += authors.back();
        return ret;    
    }
    uint32_t getPublicationYear() const { return publicationYear; }
    uint32_t getAgeRating() const { return ageRating; }
    uint64_t getIsbn() const {return isbn;}
    std::wstring getTagsString() const { 
        std::wstring ret = L"tags: ";
        for(size_t i=0; i < tags.size()-1; i++) ret = ret + tags[i] + L" ";
        ret += authors.back();
        return ret;    
    }
    std::string getImagePath() const { return imagePath; }
    uint64_t getId() const { return id; }
};

class BookDatabase {
    public:
    static BookDatabase& getInstance() {
        //Meyer's singleton
        static BookDatabase instance;   //Guaranteed to be created only once and thread-safe in C++11 and later
        return instance;
    }

    bool parse(const std::string& filename, std::string *error_string) {
        std::vector<Book> tempBookVec;
        jsonFile.open(filename, std::ios::in | std::ios::out | std::ios::app);
        if(!jsonFile) {
            if(error_string) *error_string = "failed to open file";
            return false;
        }

        json jsonData;
        try {
            jsonFile >> jsonData;
            for(auto& item : jsonData.at("books")) {
                Book tbook;
                updateBook(tbook, item);
                tempBookVec.emplace_back(tbook);
            }
        } catch(std::exception& e) {
            if(error_string) *error_string = "json parse failed";
            return false;
        }

        bookVec = std::move(tempBookVec);
        return true;
    }

    bool save() {return false;}

    bool query(uint64_t bookId, Book& retBook) {
        for(auto& item : bookVec) {
            if(item.id == bookId) {
                retBook = item;
                return true;
            }
        }
        return false;
    }

    void updateBook(Book& book, const json& jsonData) {
        book.id = jsonData.at("id");
        book.imagePath = jsonData.at("imagePath");
        book.title = jsonData.at("title").get<std::wstring>();
        book.authors = jsonData.at("authors").get<std::vector<std::wstring>>();
        book.publicationYear = jsonData.at("publicationYear");
        book.ageRating = jsonData.at("ageRating");
        book.isbn = jsonData.at("isbn");
        book.tags = jsonData.at("tags").get<std::vector<std::wstring>>();
    }

    BookDatabase(const BookDatabase&) = delete;
    BookDatabase& operator=(const BookDatabase&) = delete;

    private:
    std::fstream jsonFile;
    std::vector<Book> bookVec;

    BookDatabase() = default;
    ~BookDatabase() = default;
};


class IUIAbstract {
    protected:
    static uint64_t globalUiIdCounter;
    public:
    virtual void draw() = 0;
    virtual ~IUIAbstract() = default;
};
uint64_t IUIAbstract::globalUiIdCounter = 0;

//should only be used for short-live widget
class UICustom : public IUIAbstract {
    private:
    std::function<void(void)> func;

    public:
    UICustom(const std::function<void(void)>& tfunc) : func(tfunc) {}
    void draw() override {
        func();
    }
};

class UIBrowser : public IUIAbstract {
    static bool simpleSingleton;
    const ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyMask_ | 
                                ImGuiTabBarFlags_DrawSelectedOverline;

    std::map<std::string, std::unique_ptr<IUIAbstract>> tabMapper;
    std::vector<std::string> tabToRemoveVec;
    uint32_t tabId = 1;

    public:
    UIBrowser();
    void draw() override;
    void addNewTab(const std::string& tabName, std::unique_ptr<IUIAbstract> abstractUI);
};

UIBrowser bar;

class UIBorrowing : public IUIAbstract {

};

class UIBookView : public IUIAbstract {
    private:
    std::string widgetId;
    Book book;
    sf::Texture imageTexture;
    sf::Sprite imageSprite;

    void proceedImage(const std::string& filename) {
        assert(imageTexture.loadFromFile(filename));
        imageTexture.setSmooth(true);
        imageSprite.setTexture(imageTexture);
    }

    public:
    UIBookView(const Book& tbook) : book(tbook) {
        this->widgetId = "BookView##" + to_string(++this->globalUiIdCounter);
        proceedImage(book.getImagePath());
    }

    void draw() override {
        ImGui::BeginGroup();
        ImGui::Image(imageTexture, sf::Vector2f(480, 640));
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::PushFont(fontLarge);
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetWindowWidth());
        ImGui::TextWrapped(wideStringToString(book.getTitle()).c_str());
        ImGui::PopFont();
        ImGui::Text("ISBN: %llu", book.getIsbn());
        ImGui::Text("Author(s): %s", wideStringToString(book.getAuthorsString()).c_str());
        ImGui::Text("Publication year: %u", book.getPublicationYear());
        if(book.getAgeRating() == 0) {
            ImGui::Text("Age rating: EVERYONE");
        } else {
            ImGui::Text("Age rating: %u+", book.getAgeRating());
        }
        if(ImGui::Button(std::string("Borrow##" + widgetId).c_str())) {
            ImGui::OpenPopup("NotImplemented");
        }
        ImGui::PushFont(fontSmall);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255)); //gray color
        ImGui::TextWrapped(wideStringToString(book.getTagsString()).c_str());
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
    }
};

class UICarouselCard : public IUIAbstract {
    std::string widgetId;
    sf::Texture imageTexture;
    sf::Sprite imageSprite;
    Book book;
    
    void proceedImage(const std::string& filename) {
        assert(imageTexture.loadFromFile(filename));
        imageTexture.setSmooth(true);
        imageSprite.setTexture(imageTexture);
    }

    public:
    UICarouselCard(const Book& tbook) : book(tbook) {
        this->widgetId = "CarouselCard##" + to_string(++this->globalUiIdCounter);
        proceedImage(book.getImagePath());
    }

    void draw() override {
        imageSprite.setTexture(imageTexture);
        // ImTextureID imguiTexID = (ImTextureID) imageTexture.getNativeHandle();
        ImGui::BeginChild(widgetId.c_str(), ImVec2(350, 0), ImGuiChildFlags_Border);
        // ImGui::Image(imguiTexID, sf::Vector2f(300, 400));
        ImGui::Image(imageTexture, sf::Vector2f(300, 400));
        ImGui::PushFont(fontLarge);
        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetWindowWidth());
        ImGui::TextWideEllipsisTwoLines(book.getTitle());
        ImGui::PopFont();
        ImGui::TextWide2(book.getAuthorsString());
        if(ImGui::Button(std::string("More info##" + widgetId).c_str())) {
            bar.addNewTab(wideStringToString(std::wstring(L"Book info: " + book.getTitle())), std::unique_ptr<UIBookView>(new UIBookView(book)));
        }
        ImGui::EndChild();
    }
}; 

class CarouselCardPool {
    public:
    static CarouselCardPool& getInstance() {
        //Meyer's singleton
        static CarouselCardPool instance;
        return instance;
    }

    bool query(uint64_t bookId, UICarouselCard*& ret) {
        auto ite = pool.find(bookId);
        if(ite == pool.end()) return false;
        ret = ite->second;
        return true;
    }

    bool add(const Book& tbook) {
        auto ite = pool.find(tbook.getId());
        if(ite != pool.end()) return false;
        else {
            pool[tbook.getId()] = new UICarouselCard(tbook);
            return true;
        }
    }

    CarouselCardPool(const CarouselCardPool&) = delete;
    CarouselCardPool& operator=(const CarouselCardPool&) = delete;

    private:
    std::map<uint64_t, UICarouselCard*> pool;
    CarouselCardPool() = default;
    ~CarouselCardPool() {
        for(auto& pp : pool) {
            delete pp.second;
        }
    }
};

class UICarouselView : public IUIAbstract {
    std::string id;
    std::wstring headerStr = L"null";
    std::wstring bodyStr = L"null";
    std::vector<uint64_t> bookList;
    bool configParseError = false;

    bool parseConfig(const json& config) {
        try {
            headerStr = config.at("header").get<std::wstring>();
            bodyStr = config.at("body").get<std::wstring>();
            bookList = config.at("bookId").get<std::vector<uint64_t>>();
            for(auto& ite : bookList) {
                Book tbook;
                if(!BookDatabase::getInstance().query(ite, tbook)) {
                    return false;
                }
                CarouselCardPool::getInstance().add(tbook);
            }
        } catch(std::exception& e) {
            return false;
        }
        return true;
    }

    public:
    UICarouselView(const json& carouselConfig){
        this->id = "CarouselView##" + to_string(++this->globalUiIdCounter);
        assert(parseConfig(carouselConfig));
    }

    void draw() override {
        ImGui::PushFont(fontExtraLarge);
        ImGui::TextWide2(headerStr); //need to be in bold & larger size
        ImGui::PopFont();
        ImGui::PushFont(fontSmall);
        ImGui::TextWide2(bodyStr);
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::BeginChild(id.c_str(), ImVec2(0.0f, 500.0f), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));
        for(size_t i=0; i < bookList.size(); i++) {
            UICarouselCard *ptr;
            if(!CarouselCardPool::getInstance().query(bookList[i], ptr)) {
                std::cout << "Load book id " << bookList[i] << " failed\n"; 
            }
            ptr->draw();
            if(i < bookList.size()-1) ImGui::SameLine();
        }
        ImGui::PopStyleVar();
        ImGui::EndChild();
    }
};

class UICarouselView2 : public IUIAbstract {
    int childId = 1;

    public: 
    UICarouselView2() {

    }

    std::string childIdString(int i) {
        return std::string("Mychild##") + to_string(i);
    }

    void draw() override {
        ImGui::BeginChild("UICarouselView2", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
        if(ImGui::Button("Spawn")) {
            childId++;
        }
        ImGui::Separator();
        // ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));
        for(int i=0; i < childId; i++) {
            ImGui::BeginChild(childIdString(i).c_str(), ImVec2(100, 100), ImGuiChildFlags_Border);
            ImGui::Text("Child ID: %d", i);
            ImGui::Text("Lorem ipsum dolor sit amet.");
            if (ImGui::Button("Inside Button")) {
                std::cout << "Button clicked in Child " << i << std::endl;
            }
            ImGui::EndChild();
            if(i < childId-1) ImGui::SameLine();
        }
        ImGui::PopStyleVar();
        ImGui::EndChild();
    }
};


UIBrowser::UIBrowser() {
    if(simpleSingleton) throw std::runtime_error("only one instance allowed");
    simpleSingleton = true;
}

void UIBrowser::draw() {
    if(ImGui::BeginTabBar("MyTabs", tabFlags)) {
        if(ImGui::BeginTabItem("Homepage##0")) {
            if (ImGui::Button("Add New Tab")) {
                ifstream infile("./data/book/carousel.json");
                json j;
                infile >> j;
                j = j.at("carousel");
                for(auto& ite : j) {
                    addNewTab("shit library", std::unique_ptr<UICarouselView>(new UICarouselView(ite)));
                }

                // addNewTab("test", std::unique_ptr<UICarouselView2>(new UICarouselView2()));
            }
            ImGui::EndTabItem();
        }
        
        for(auto& pp : tabMapper) {
            bool tabOpenFlag = true;
            if(ImGui::BeginTabItem(pp.first.c_str(), &tabOpenFlag)) {
                pp.second->draw();
                ImGui::EndTabItem();
            }

            if(!tabOpenFlag) tabToRemoveVec.push_back(pp.first);
        }
    }
    ImGui::EndTabBar();

    for(auto& tstr : tabToRemoveVec) {
        tabMapper.erase(tstr);
    }
    tabToRemoveVec.clear();
}

void UIBrowser::addNewTab(const std::string& tabName, std::unique_ptr<IUIAbstract> abstractUI) {
    std::string internalTabName = tabName + "##" + std::to_string(tabId);
    tabMapper[internalTabName] = std::move(abstractUI);
    tabId++;
}

bool UIBrowser::simpleSingleton = false;

class FontLoader {
    public:
    FontLoader& get() {
        static FontLoader instance;
        return instance;
    }

    FontLoader(const FontLoader&) = delete;
    FontLoader& operator=(const FontLoader&) = delete;

    private:
    ImFontConfig config;

    void init() {
        config.MergeMode = true;
        config.PixelSnapH = true;
    }

    FontLoader() = default;
    ~FontLoader() = default;
};

void LoadFont() {
    const std::string regularFontFile = "./data/font/NotoSansDisplay-Medium.ttf";
    const std::string japaneseFontFile = "./data/font/NotoSansJP-Medium.ttf";
    const std::string emojiFontFile = "./data/font/NotoEmoji-Medium.ttf";

    ImGuiIO& io = ImGui::GetIO();
    UNREFERENCED_PARAMETER(io);
    io.Fonts->Clear();
    ImFontConfig fontConfig, fontMergingConfig;
    fontConfig.PixelSnapH = true;
    fontConfig.FontDataOwnedByAtlas = false;    //free memory on our own

    fontMergingConfig.MergeMode = true;
    fontMergingConfig.PixelSnapH = true;
    fontMergingConfig.FontDataOwnedByAtlas = false;

    int regularFontMemSize, japaneseFontMemSize, emojiFontMemSize;
    assert(LoadFile(regularFontFile, GlobalMemory::get().regularFontMem, regularFontMemSize));
    assert(LoadFile(japaneseFontFile, GlobalMemory::get().japaneseFontMem, japaneseFontMemSize));
    assert(LoadFile(emojiFontFile, GlobalMemory::get().emojiFontMem, emojiFontMemSize));

    fontMedium = io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().regularFontMem, regularFontMemSize, 18.0f, &fontConfig, regularGlyphRange);
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().japaneseFontMem, japaneseFontMemSize, 18.0f, &fontMergingConfig, io.Fonts->GetGlyphRangesJapanese());
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().emojiFontMem, emojiFontMemSize, 18.0f, &fontMergingConfig, emojiGlyphRange);
    fontLarge = io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().regularFontMem, regularFontMemSize, 24.0f, &fontConfig, regularGlyphRange);
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().japaneseFontMem, japaneseFontMemSize, 24.0f, &fontMergingConfig, io.Fonts->GetGlyphRangesJapanese());
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().emojiFontMem, emojiFontMemSize, 24.0f, &fontMergingConfig, emojiGlyphRange);
    fontExtraLarge = io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().regularFontMem, regularFontMemSize, 32.0f, &fontConfig, regularGlyphRange);
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().japaneseFontMem, japaneseFontMemSize, 32.0f, &fontMergingConfig, io.Fonts->GetGlyphRangesJapanese());
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().emojiFontMem, emojiFontMemSize, 32.0f, &fontMergingConfig, emojiGlyphRange);
    fontExtraSmall = io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().regularFontMem, regularFontMemSize, 12.0f, &fontConfig, regularGlyphRange);
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().japaneseFontMem, japaneseFontMemSize, 12.0f, &fontMergingConfig, io.Fonts->GetGlyphRangesJapanese());
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().emojiFontMem, emojiFontMemSize, 12.0f, &fontMergingConfig, emojiGlyphRange);
    fontSmall = io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().regularFontMem, regularFontMemSize, 14.0f, &fontConfig, regularGlyphRange);
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().japaneseFontMem, japaneseFontMemSize, 14.0f, &fontMergingConfig, io.Fonts->GetGlyphRangesJapanese());
    (void) io.Fonts->AddFontFromMemoryTTF(GlobalMemory::get().emojiFontMem, emojiFontMemSize, 14.0f, &fontMergingConfig, emojiGlyphRange);

    assert(ImGui::SFML::UpdateFontTexture());
}

int main() {
    window.create(sf::VideoMode(800, 600), "ImGui SFML GUI");
    window.setVerticalSyncEnabled(true);
    assert(ImGui::SFML::Init(window, false));
    ImGuiIO& io = ImGui::GetIO();
    UNREFERENCED_PARAMETER(io);
    io.IniFilename = nullptr;
    LoadFont();

    ImGui::StyleColorsLight();

    const float globalScaleFactor = 1.0f;
    // io.FontGlobalScale = globalScaleFactor;
    ImGui::GetStyle().ScaleAllSizes(globalScaleFactor);

    sf::Clock deltaClock;

    assert(BookDatabase::getInstance().parse("./data/book/book.json", nullptr));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            } else if(event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
                window.setView(sf::View(visibleArea));
                windowSize.x = (float) event.size.width;
                windowSize.y = (float) event.size.height;
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(windowSize);
        // ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar);
        ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        bar.draw();

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New", "Ctrl+N")) {
                    std::cout << "File -> New clicked!" << std::endl;
                }
                if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                    std::cout << "File -> Open clicked!" << std::endl;
                }
                ImGui::Separator();

                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    std::cout << "File -> Save clicked!" << std::endl;
                }
                if (ImGui::MenuItem("Save As...")) {
                    std::cout << "File -> Save As clicked!" << std::endl;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    window.close();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();

        // ImGui::ShowDemoWindow(nullptr);
        
        window.clear(sf::Color(0, 0, 128, 255));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}