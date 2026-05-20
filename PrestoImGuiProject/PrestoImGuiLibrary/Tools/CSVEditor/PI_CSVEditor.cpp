#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>
#include <implot.h>

#include "PI_CSVEditor.h"

void PI_CSVEditor::Draw(std::string_view label, bool& open)
{
    constexpr static auto window_flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    constexpr static auto window_size = ImVec2(1280.0F, 720.0F);
    constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);


    if (!ImGui::Begin(label.data(), &open, window_flags))
    {
        ImGui::End();
        return;
    }

    DrawSizeButtons();
    ImGui::Separator();
    DrawIoButtons();
    ImGui::Separator();
    DrawTable();

    ImGui::End();
}

void PI_CSVEditor::DrawSizeButtons()
{
    ImGui::Text("Rows");
    ImGui::SliderInt("##rows", &numRows, 0, maxNumRows);

    ImGui::SameLine();
    if (ImGui::Button("+ Row") && numRows < maxNumRows)
        ++numRows;

    ImGui::SameLine();
    if (ImGui::Button("- Row") && numRows > 0)
        --numRows;

    ImGui::Text("Cols");
    ImGui::SliderInt("##cols", &numCols, 0, maxNumCols);

    ImGui::SameLine();
    if (ImGui::Button("+ Col") && numCols < maxNumCols)
        ++numCols;

    ImGui::SameLine();
    if (ImGui::Button("- Col") && numCols > 0)
        --numCols;

    SyncSize();
}

void PI_CSVEditor::DrawIoButtons()
{
    if (ImGui::Button("Save"))
        ImGui::OpenPopup("Save File");

    ImGui::SameLine();

    if (ImGui::Button("Load"))
        ImGui::OpenPopup("Load File");

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        data.clear();
        numRows = 0;
        numCols = 0;
    }

    DrawSavePopup();
    DrawLoadPopup();
}

void PI_CSVEditor::DrawTable()
{
    constexpr static auto table_flags =
        ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuter;

    static auto row_clicked = 0;
    static auto col_clicked = 0;

    if (numCols == 0)
        return;

    ImGui::BeginTable("Table", numCols, table_flags);

    for (std::int32_t col = 0; col < numCols; ++col)
    {
        std::string col_name(1, static_cast<char>('A' + col));
        ImGui::TableSetupColumn(col_name.c_str(), ImGuiTableColumnFlags_WidthFixed,
            1280.0f / numCols);
    }

    ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
    for (std::int32_t col = 0; col < numCols; ++col)
        PlotCellValue("%c", 'A' + col);

    for (std::int32_t row = 0; row < numRows; ++row)
    {
        for (std::int32_t col = 0; col < numCols; ++col)
        {
            PlotCellValue("%f", data[row][col]);
            if (ImGui::IsItemClicked())
            {
                ImGui::OpenPopup("Change Value");
                row_clicked = row;
                col_clicked = col;
            }
            else if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Cell: (%d, %d)", row, col);
            }
        }
        ImGui::TableNextRow();
    }

    DrawValuePopup(row_clicked, col_clicked);

    ImGui::EndTable();
}

void PI_CSVEditor::DrawSavePopup()
{
    const auto esc_pressed = ImGui::IsKeyPressed(ImGuiKey_Escape);

    SetPopupLayout();
    if (ImGui::BeginPopupModal("Save File", nullptr, popUpFlags))
    {
        ImGui::InputText("Filename", filenameBuffer, sizeof(filenameBuffer));

        if (ImGui::Button("Save", popUpButtonSize))
        {
            SaveToCsvFile(filenameBuffer);
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

void PI_CSVEditor::DrawLoadPopup()
{
    const auto esc_pressed = ImGui::IsKeyPressed(ImGuiKey_Escape);

    SetPopupLayout();
    if (ImGui::BeginPopupModal("Load File", nullptr, popUpFlags))
    {
        ImGui::InputText("Filename", filenameBuffer, sizeof(filenameBuffer));

        if (ImGui::Button("Load", popUpButtonSize))
        {
            LoadFromCsvFile(filenameBuffer);
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

void PI_CSVEditor::DrawValuePopup(const int row, const int col)
{
    static char buffer[64] = { '\0' };

    const auto esc_pressed = ImGui::IsKeyPressed(ImGuiKey_Escape);

    SetPopupLayout();
    if (ImGui::BeginPopupModal("Change Value", nullptr, popUpFlags))
    {
        const auto label = fmt::format("##{}_{}", row, col);
        ImGui::InputText(label.data(), buffer, sizeof(buffer));

        if (ImGui::Button("Save"))
        {
            try {
                data[row][col] = std::stof(buffer);
            }
            catch (...) {
                data[row][col] = 0.0f;
            }

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

void PI_CSVEditor::SaveToCsvFile(std::string_view filename)
{
    auto out = std::ofstream{ filename.data() };

    if (!out || !out.is_open())
        return;

    for (std::int32_t row = 0; row < numRows; ++row)
    {
        for (std::int32_t col = 0; col < numCols; ++col)
        {
            out << data[row][col];
            out << ',';
        }
        out << '\n';
    }

    out.close();
}

void PI_CSVEditor::LoadFromCsvFile(std::string_view filename)
{
    std::ifstream in(filename.data());
    if (!in) return;

    data.clear();

    std::string line;
    size_t maxCols = 0;

    while (std::getline(in, line))
    {
        // --- remove BOM (Excel UTF-8 issue) ---
        if (line.size() >= 3 &&
            (unsigned char)line[0] == 0xEF &&
            (unsigned char)line[1] == 0xBB &&
            (unsigned char)line[2] == 0xBF)
        {
            line.erase(0, 3);
        }

        // --- remove Windows CR ---
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        std::vector<float> row;
        std::string token;

        // --- detect Excel delimiter (tab / ; / ,) ---
        char delimiter = ',';
        if (line.find('\t') != std::string::npos) delimiter = '\t';
        else if (line.find(';') != std::string::npos) delimiter = ';';

        std::stringstream ss(line);

        while (std::getline(ss, token, delimiter))
        {
            // --- trim spaces (Excel often pads columns visually) ---
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);

            // --- remove quotes ---
            token.erase(std::remove(token.begin(), token.end(), '\"'), token.end());

            // --- empty cell ---
            if (token.empty())
            {
                row.push_back(0.0f);
                continue;
            }

            // --- convert safely ---
            try
            {
                row.push_back(std::stof(token));
            }
            catch (...)
            {
                // header text or garbage -> 0
                row.push_back(0.0f);
            }
        }

        maxCols = std::max(maxCols, row.size());
        data.push_back(std::move(row));
    }

    numRows = static_cast<int>(data.size());
    numCols = static_cast<int>(maxCols);

    SyncSize();
}

void PI_CSVEditor::SyncSize()
{
    data.resize(numRows);

    for (auto& row : data)
        row.resize(numCols, 0.0f);
}

template <typename T>
void PI_CSVEditor::PlotCellValue(std::string_view formatter, const T value)
{
    ImGui::TableNextColumn();
    ImGui::Text(formatter.data(), value);
}

void PI_CSVEditor::SetPopupLayout()
{
    ImGui::SetNextWindowSize(popUpSize);
    ImGui::SetNextWindowPos(popUpPos);
}