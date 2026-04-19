#pragma once
#include <string>
#include <filesystem>
#include <imgui.h>

namespace fs = std::filesystem;
class PI_TextEditorViewer
{
public:
    PI_TextEditorViewer() : currentFilename({})
    {
        // fills a block of memory with a specific byte value
        std::memset(textBuffer, 0, bufferSize);
    }
    void Draw();

    static constexpr auto bufferSize = std::size_t{ 1024 };
    static constexpr auto popUpFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    static constexpr auto popUpSize = ImVec2(300.f, 150.f);
    static constexpr auto popUpButtonSize = ImVec2(120.f, 0.f);

private:
    void DrawMenu();
    void DrawLoadPopup();
    void DrawSavePopup();
    void DrawContent();
    void DrawInfo();

    void SaveToFile(std::string_view filename);
    void LoadFromFile(std::string_view filename);
    std::string GetFileExtension(std::string_view filename);

    char textBuffer[bufferSize] = { 0 };
    std::string currentFilename;
};