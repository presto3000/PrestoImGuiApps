#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class PI_FileBrowserView
{
public:
    void Draw();

private:
    void DrawMenu();
    void DrawContent();
    void DrawActions();
    void DrawFilter();

    void openFileWithDefaultEditor();
    void renameFilePopup();
    void deleteFilePopup();

private:
    fs::path currentPath = fs::current_path();
    fs::path selectedEntry;
};