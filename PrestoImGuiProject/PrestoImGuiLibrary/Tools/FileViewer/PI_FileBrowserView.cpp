#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "PI_FileBrowserView.h"

void PI_FileBrowserView::Draw()
{
    ImGui::Begin("File Browser");

    DrawMenu();
    ImGui::Separator();

    DrawContent();
    ImGui::Separator();

    DrawActions();
    ImGui::Separator();

    DrawFilter();

    ImGui::End();
}

void PI_FileBrowserView::DrawMenu()
{
    if (ImGui::Button("Go Up"))
    {
        if (currentPath.has_parent_path())
            currentPath = currentPath.parent_path();
    }

    ImGui::SameLine();
    ImGui::Text("Current Directory: %s", currentPath.string().c_str());
}

void PI_FileBrowserView::DrawContent()
{
    for (const auto& entry : fs::directory_iterator(currentPath))
    {
        bool isSelected = (entry.path() == selectedEntry);
        bool isDir = entry.is_directory();
        bool isFile = entry.is_regular_file();

        std::string name = entry.path().filename().string();

        if (isDir)  name = "[DIR] " + name;
        if (isFile) name = "[FILE] " + name;

        if (ImGui::Selectable(name.c_str(), isSelected))
        {
            if (isDir)
                currentPath /= entry.path().filename();
            else
                selectedEntry = entry.path();
        }
    }
}


void PI_FileBrowserView::DrawActions()
{
    if (fs::is_directory(selectedEntry))
        ImGui::Text("Selected Directory: %s", selectedEntry.string().c_str());
    else if (fs::is_regular_file(selectedEntry))
        ImGui::Text("Selected File: %s", selectedEntry.string().c_str());
    else
        ImGui::Text("No selection");

    if (fs::is_regular_file(selectedEntry))
    {
        if (ImGui::Button("Open"))
            openFileWithDefaultEditor();
    }

    ImGui::SameLine();

    if (ImGui::Button("Rename"))
        ImGui::OpenPopup("RenamePopup");

    ImGui::SameLine();

    if (ImGui::Button("Delete"))
        ImGui::OpenPopup("DeletePopup");

    renameFilePopup();
    deleteFilePopup();
}

void PI_FileBrowserView::DrawFilter()
{
    ImGui::Text("Filter by extension");
    ImGui::SameLine();

    static char filterBuffer[256] = "";
    ImGui::InputText("##filter", filterBuffer, sizeof(filterBuffer));

    if (std::strlen(filterBuffer) == 0)
        return;

    size_t count = 0;

    for (const auto& entry : fs::directory_iterator(currentPath))
    {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension().string() == filterBuffer)
            count++;
    }

    ImGui::Text("Filtered files: %zu", count);
}

void PI_FileBrowserView::openFileWithDefaultEditor()
{
    if (!fs::is_regular_file(selectedEntry))
        return;

    std::string path = selectedEntry.string();

#if defined(_WIN32)
    std::system(("start \"\" \"" + path + "\"").c_str());
#elif defined(__APPLE__)
    std::system(("open \"" + path + "\"").c_str());
#else
    std::system(("xdg-open \"" + path + "\"").c_str());
#endif
}

void PI_FileBrowserView::renameFilePopup()
{
    if (ImGui::BeginPopupModal("RenamePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char newName[256] = "";

        ImGui::InputText("New Name", newName, sizeof(newName));

        if (ImGui::Button("OK"))
        {
            if (!selectedEntry.empty())
            {
                fs::path newPath = selectedEntry.parent_path() / newName;

                try
                {
                    fs::rename(selectedEntry, newPath);
                    selectedEntry = newPath;
                }
                catch (const fs::filesystem_error& e)
                {
                    std::cerr << e.what() << "\n";
                }
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

void PI_FileBrowserView::deleteFilePopup()
{
    if (ImGui::BeginPopupModal("DeletePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Delete:");
        ImGui::Text("%s", selectedEntry.string().c_str());

        if (ImGui::Button("OK"))
        {
            try
            {
                fs::remove(selectedEntry);
                selectedEntry.clear();
            }
            catch (const fs::filesystem_error& e)
            {
                std::cerr << e.what() << "\n";
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}