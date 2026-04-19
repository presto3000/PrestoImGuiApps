#include "PI_TextEditorViewer.h"
#include <imgui.h>
#include <fstream>
#include <iostream>

void PI_TextEditorViewer::Draw()
{
    ImGui::Begin("Text Editor");

    ImGui::Text("Simple Text Editor");

    ImGui::Separator();

    DrawMenu();
    DrawContent();
    DrawInfo();

    ImGui::End();
}

void PI_TextEditorViewer::SaveToFile(std::string_view filename)
{
    std::ofstream out(std::string(filename), std::ios::binary);
    if (!out)
        return;

    out.write(textBuffer, std::strlen(textBuffer));
}

void PI_TextEditorViewer::LoadFromFile(std::string_view filename)
{
    std::ifstream in(std::string(filename), std::ios::binary);
    if (!in)
        return;

    in.read(textBuffer, bufferSize - 1);

    // ensure null termination
    std::streamsize bytesRead = in.gcount();
    textBuffer[bytesRead] = '\0';
    currentFilename = filename;
}

std::string PI_TextEditorViewer::GetFileExtension(std::string_view filename)
{
    const auto file_path = fs::path(filename);
    return file_path.extension().string();
}

void PI_TextEditorViewer::DrawMenu()
{
    const auto ctrl_pressed = ImGui::GetIO().KeyCtrl;
    const bool esc_pressed = ImGui::IsKeyPressed(ImGuiKey_Escape);
    const bool s_pressed = ImGui::IsKeyPressed(ImGuiKey_S);
    const bool l_pressed = ImGui::IsKeyPressed(ImGuiKey_L);
    const bool c_pressed = ImGui::IsKeyPressed(ImGuiKey_C);

    if (ImGui::Button("Save") || (ctrl_pressed && s_pressed))
    {
        ImGui::OpenPopup("Save File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Load") || (ctrl_pressed && l_pressed))
    {
        ImGui::OpenPopup("Load File");
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        std::memset(textBuffer, 0, bufferSize);
    }

    DrawSavePopup();
    DrawLoadPopup();
}

void PI_TextEditorViewer::DrawSavePopup()
{
    const bool esc_pressed = ImGui::IsKeyPressed(ImGuiKey_Escape);

    ImGui::SetNextWindowSize(popUpSize);
    ImGui::SetNextWindowPos(ImVec2(
        ImGui::GetIO().DisplaySize.x * 0.5f,
        ImGui::GetIO().DisplaySize.y * 0.5f));

    if (ImGui::BeginPopupModal("Save File", nullptr, popUpFlags))
    {
        static char filename[256] = "";
        ImGui::InputText("Filename", filename, sizeof(filename));

        if (ImGui::Button("Save", popUpButtonSize))
        {
            SaveToFile(filename);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", popUpButtonSize) || esc_pressed)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

}

void PI_TextEditorViewer::DrawLoadPopup()
{
    ImGui::SetNextWindowSize(popUpSize);
    ImGui::SetNextWindowPos(ImVec2(
        ImGui::GetIO().DisplaySize.x * 0.5f,
        ImGui::GetIO().DisplaySize.y * 0.5f));

    const bool esc_pressed = ImGui::IsKeyPressed(ImGuiKey_Escape);
    if (ImGui::BeginPopupModal("Load File", nullptr, popUpFlags))
    {
        static char filename[256] = "";
        ImGui::InputText("Filename", filename, sizeof(filename));

        if (ImGui::Button("Load", popUpButtonSize))
        {
            LoadFromFile(filename);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel") || esc_pressed)
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void PI_TextEditorViewer::DrawContent()
{
    static constexpr float height = 650.f;
    static constexpr float lineNumberWidth = 50.f;

    static constexpr auto inputTextFlags =
        ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_NoHorizontalScroll;

    ImGuiStyle& style = ImGui::GetStyle();

    // Subtract padding instead of adding
    float frameExtraY = style.FramePadding.y * 6.0f;

    ImVec2 inputTextSize = ImVec2(1200.f, height);
    ImVec2 lineNumberSize = ImVec2(lineNumberWidth, height - frameExtraY);

    ImGui::BeginChild("LineNumbers", lineNumberSize, true);

    const auto line_count = std::count(textBuffer, textBuffer + bufferSize, '\n') + 1;
    for (int i = 1; i <= line_count; ++i)
    {
        ImGui::Text("%d", i);
    }

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::InputTextMultiline("###text", textBuffer, bufferSize, inputTextSize, inputTextFlags);
}

void PI_TextEditorViewer::DrawInfo()
{
    if (currentFilename.empty())
    {
        ImGui::Text("No file loaded");
        return;
    }

    const auto file_extension = GetFileExtension(currentFilename);
    ImGui::Text("Opened file: %s | File extension : %s", currentFilename.data(), file_extension.data());


}