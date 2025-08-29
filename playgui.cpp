#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

static int inputCallback(ImGuiInputTextCallbackData* data)
{
    // Force SDL3 to know where the caret is (for IME positioning)
    if (ImGui::IsItemActive())
    {
        ImVec2 cursor_screen = ImGui::GetCursorScreenPos();
        float font_size = ImGui::GetFontSize();

        // Calculate approximate caret rect
        SDL_Rect area;
        area.x = (int)(cursor_screen.x + ImGui::GetCursorPosX());
        area.y = (int)(cursor_screen.y + ImGui::GetCursorPosY());
        area.w = 1;
        area.h = (int)font_size;

        SDL_Window* window = (SDL_Window*)ImGui::GetMainViewport()->PlatformHandle;
        if (window)
            SDL_SetTextInputArea(window, &area, 0);
    }
    return 0;
}

int main(int, char**)
{
    // SDL init
    SDL_SetHint(SDL_HINT_IME_IMPLEMENTED_UI, "0"); // Use Windows' native IME UI
    if (!SDL_Init(SDL_INIT_VIDEO))
        return 1;

    SDL_Window* window = SDL_CreateWindow("SDL3 + ImGui + IME demo", 1280, 720,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    // Load font with Vietnamese glyphs
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 18.0f, nullptr,
                                 io.Fonts->GetGlyphRangesVietnamese());

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    static char buf[2048] = {0};
    bool running = true;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL3_ProcessEvent(&e);
            if (e.type == SDL_EVENT_QUIT)
                running = false;
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("IME Test (Vietnamese)");
        ImGui::TextWrapped("Focus the input below and enable Windows Vietnamese IME "
                           "(Telex/VNI). The white composition box and candidate list "
                           "should appear near the caret.");

        ImGui::InputTextMultiline("##text", buf, IM_ARRAYSIZE(buf),
                                  ImVec2(-FLT_MIN, 200.0f),
                                  ImGuiInputTextFlags_CallbackAlways,
                                  inputCallback);
        ImGui::End();

        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
