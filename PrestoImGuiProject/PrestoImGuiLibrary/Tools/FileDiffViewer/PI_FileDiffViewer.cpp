#include "PI_FileDiffViewer.h"
#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>
#include <fstream>
#include "imgui_stdlib.h"


void PI_FileDiffViewer::Draw()
{
    ImGui::Begin("File Diff Viewer");

    DrawSelection();
    DrawDiffView();
    DrawStats();

    ImGui::End();
}

void PI_FileDiffViewer::DrawSelection()
{
    ImGui::InputText("Left", &filePath1);
    ImGui::SameLine();
    if (ImGui::Button("Save###Left"))
    {
        SaveFileContent(filePath1, fileContent1);
    }

    ImGui::InputText("Right", &filePath2);
    ImGui::SameLine();
    if (ImGui::Button("Save###Right"))
    {
        SaveFileContent(filePath2, fileContent2);
    }

    if (ImGui::Button("Compare"))
    {
        fileContent1 = LoadFileContent(filePath1);
        fileContent2 = LoadFileContent(filePath2);
        CreateDiff();
    }
}

void PI_FileDiffViewer::DrawDiffView()
{
    const float height = 500.f;
    const float middle_width = 50.f;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("Parent", ImVec2(0, height), true);

    const float available_width = ImGui::GetContentRegionAvail().x;
    const float side_width = (available_width - middle_width) * 0.5f;

    const size_t count = diffResult1.size();
    const float line_height = ImGui::GetTextLineHeightWithSpacing();
    const ImVec2 button_size(15.f, line_height);

    // --- LEFT PANEL ---
    ImGui::BeginChild("Diff1", ImVec2(side_width, height), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetScrollY(m_scroll);
    for (size_t i = 0; i < count; ++i)
    {
        if (!diffResult1[i].empty())
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", fileContent1[i].c_str());
        else
            ImGui::Text("%s", fileContent1[i].c_str());
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // --- MIDDLE PANEL (SWAP BUTTONS) ---
    ImGui::BeginChild("Swap", ImVec2(middle_width, height), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetScrollY(m_scroll);
    for (size_t i = 0; i < count; ++i)
    {
        if (!diffResult1[i].empty() || !diffResult2[i].empty())
        {
            std::string left_label = "<##" + std::to_string(i);
            std::string right_label = ">##" + std::to_string(i);

            // < means "push left side TO right"
            if (ImGui::Button(left_label.c_str(), button_size))
            {
                fileContent1[i] = fileContent2[i];
                CreateDiff();
            }
            ImGui::SameLine();
            // > means "push right side TO left"
            if (ImGui::Button(right_label.c_str(), button_size))
            {
                fileContent2[i] = fileContent1[i];
                CreateDiff();
            }
        }
        else
        {
            ImGui::Dummy(ImVec2(middle_width, line_height));
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // --- RIGHT PANEL ---
    ImGui::BeginChild("Diff2", ImVec2(side_width, height), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetScrollY(m_scroll);
    for (size_t i = 0; i < count; ++i)
    {
        if (!diffResult2[i].empty())
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", fileContent2[i].c_str());
        else
            ImGui::Text("%s", fileContent2[i].c_str());
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // --- SCROLLBAR ---
    ImGui::BeginChild("Scrollbar", ImVec2(0, height), false);
    ImGui::SetScrollY(m_scroll);
    ImGui::Dummy(ImVec2(0, count * line_height));
    m_scroll = ImGui::GetScrollY();
    ImGui::EndChild();

    ImGui::EndChild(); // Parent
    ImGui::PopStyleVar();
}

void PI_FileDiffViewer::DrawStats()
{
    auto diff_lines_count = std::size_t{ 0 };
    for (const auto& line : diffResult1)
    {
        if (!line.empty())
            ++diff_lines_count;
    }

    ImGui::Separator();
    ImGui::Text("Diff lines count: %zu", diff_lines_count);
}


PI_FileDiffViewer::FileContent PI_FileDiffViewer::LoadFileContent(std::string_view file_path)
{
    auto content = FileContent{};
    auto in = std::ifstream(file_path.data());

    if (in.is_open())
    {
        auto line = std::string{};
        while (std::getline(in, line))
        {
            content.push_back(line);
        }
    }

    in.close();

    return content;
}

void PI_FileDiffViewer::SaveFileContent(std::string_view file_path, FileContent& file_content)
{
    auto out = std::ofstream(file_path.data());

    if (out.is_open())
    {
        for (const auto& line : file_content)
        {
            out << line << "\n";
        }
    }
    out.close();
}

void PI_FileDiffViewer::CreateDiff()
{
    diffResult1.clear();
    diffResult2.clear();

    const auto max_num_lines = std::max(fileContent1.size(), fileContent2.size());

    while (fileContent1.size() < max_num_lines)
        fileContent1.push_back("");
    while (fileContent2.size() < max_num_lines)
        fileContent2.push_back("");

    for (std::size_t i = 0; i < max_num_lines; ++i)
    {
        if (fileContent1[i] != fileContent2[i])
        {
            diffResult1.push_back("*");  // marker, not the content
            diffResult2.push_back("*");
        }
        else
        {
            diffResult1.push_back("");
            diffResult2.push_back("");
        }
    }
}