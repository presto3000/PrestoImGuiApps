#include <algorithm>
#include <chrono>
#include <fstream>

#include <fmt/format.h>
#include <imgui.h>
#include "PI_Calendar.h"

using Date = PI_Calendar::Date;

PI_Calendar::PI_Calendar()
{
    const auto today = std::chrono::floor<std::chrono::days>(
        std::chrono::system_clock::now()
    );

    selectedDate = std::chrono::year_month_day{ today };
}

void PI_Calendar::Draw(std::string_view label, bool& open)
{
    constexpr auto window_flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowSize({ 1280.0f, 720.0f });
    ImGui::SetNextWindowPos({ 0.0f, 0.0f });

    if (open)
    {
        ImGui::Begin(label.data(), &open, window_flags);

        DrawCalendar();
        ImGui::Separator();
        DrawDateCombo();
        ImGui::Separator();
        DrawMeetingList();

        if (addMeetingWindowOpen)
            DrawAddMeetingWindow();

        ImGui::End();
    }
}

void PI_Calendar::DrawCalendar()
{
    ImGui::BeginChild("##calendar", { 0, 400 }, false);

    const auto now = std::chrono::floor<std::chrono::days>(
        std::chrono::system_clock::now()
    );
    const auto today = std::chrono::year_month_day{ now };

    std::chrono::year y = selectedDate.year();

    const Date sys_today = std::chrono::sys_days{ today };
    const Date sys_selected = std::chrono::sys_days{ selectedDate };


    ImGui::SameLine();

    // RIGHT PANEL (calendar grid)
    ImGui::BeginChild("##days", { 0, 0 }, false);

    for (int m = 1; m <= 12; ++m)
    {
        ImGui::TextUnformatted(monthNames[m - 1].data());

        for (int d = 1; d <= 31; ++d)
        {
            std::chrono::year_month_day date{
                y,
                std::chrono::month(m),
                std::chrono::day(d)
            };

            if (!date.ok())
                break;

            Date sys_date = std::chrono::sys_days{ date };

            std::string label =
                std::to_string(d) + "##" +
                std::to_string(m) + "_" +
                std::to_string(int(y));

            ImVec4 color = { 1,1,1,1 };

            if (sys_date == sys_today)
                color = { 0, 1, 0, 1 };
            else if (sys_date == sys_selected)
                color = { 1, 1, 0, 1 };
            else if (meetings.contains(sys_date))
                color = { 1, 0, 0, 1 };

            ImGui::PushStyleColor(ImGuiCol_Text, color);

            if (ImGui::Button(label.c_str(), { 30, 30 }))
            {
                selectedDate = date;
            }

            ImGui::PopStyleColor();

            ImGui::SameLine();
        }

        ImGui::NewLine();
    }

    ImGui::EndChild();

    ImGui::EndChild();
}

void PI_Calendar::DrawDateCombo()
{
    ImGui::Text("Select a date:");

    // DAY
    ImGui::PushItemWidth(50);

    {
        auto current_day = unsigned(selectedDate.day());
        auto label = std::to_string(current_day);

        if (ImGui::BeginCombo("##day", label.c_str()))
        {
            auto year = selectedDate.year();
            auto month = selectedDate.month();

            for (int d = 1; d <= 31; ++d)
            {
                std::chrono::year_month_day temp{
                    year,
                    month,
                    std::chrono::day(d)
                };

                if (!temp.ok())
                    break;

                auto text = std::to_string(d);

                if (ImGui::Selectable(text.c_str(), d == current_day))
                {
                    selectedDate = temp;
                }
            }

            ImGui::EndCombo();
        }

    }

    ImGui::PopItemWidth();
    ImGui::SameLine();

    // MONTH
    ImGui::PushItemWidth(100);

    {
        auto label = monthNames[unsigned(selectedDate.month()) - 1];

        if (ImGui::BeginCombo("##month", label.data()))
        {
            for (int m = 1; m <= 12; ++m)
            {
                std::chrono::year_month_day temp{
                    selectedDate.year(),
                    std::chrono::month(m),
                    selectedDate.day()
                };

                if (!temp.ok())
                    break;

                if (ImGui::Selectable(monthNames[m - 1].data(),
                    m == unsigned(selectedDate.month())))
                {
                    selectedDate = temp;
                }
            }

            ImGui::EndCombo();
        }
    }

    ImGui::PopItemWidth();
    ImGui::SameLine();

    // YEAR
    ImGui::PushItemWidth(60);

    {
        auto label = std::to_string(int(selectedDate.year()));

        if (ImGui::BeginCombo("##year", label.c_str()))
        {
            for (int y = minYear; y <= maxYear; ++y)
            {
                std::chrono::year_month_day temp{
                    std::chrono::year(y),
                    selectedDate.month(),
                    selectedDate.day()
                };

                if (!temp.ok())
                    break;

                if (ImGui::Selectable(std::to_string(y).c_str(),
                    y == int(selectedDate.year())))
                {
                    selectedDate = temp;
                }
            }

            ImGui::EndCombo();
        }
    }

    ImGui::PopItemWidth();

    if (ImGui::Button("Add Meeting"))
        addMeetingWindowOpen = true;
}

void PI_Calendar::DrawMeetingList()
{
    const Date key = std::chrono::sys_days{ selectedDate };

    auto it = meetings.find(key);

    if (meetings.empty())
    {
        ImGui::Text("No meetings at all.");
        return;
    }

    ImGui::Text("Meetings on selected date:");

    if (it == meetings.end())
    {
        ImGui::Text("No meetings for this day.");
        return;
    }

    auto& vec = it->second;

    if (vec.empty())
    {
        ImGui::Text("No meetings for this day.");
        return;
    }

    for (auto& meeting : vec)
    {
        ImGui::BulletText("%s", meeting.name.c_str());

        if (ImGui::IsItemClicked())
        {
            std::erase(vec, meeting);

            if (vec.empty())
                meetings.erase(it);

            break;
        }
    }
}

void PI_Calendar::DrawAddMeetingWindow()
{
    static char buffer[128]{};

    ImGui::Begin("Add Meeting", &addMeetingWindowOpen,
        meetingWindowFlags);

    ImGui::InputText("Name", buffer, sizeof(buffer));

    if (ImGui::Button("Save"))
    {
        Date key = std::chrono::sys_days{ selectedDate };
        meetings[key].push_back(Meeting{ buffer });

        buffer[0] = '\0';
        addMeetingWindowOpen = false;
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
        addMeetingWindowOpen = false;

    ImGui::End();
}

void PI_Calendar::SaveMeetingsToFile(std::string_view filename)
{
    std::ofstream out(filename.data(), std::ios::binary);
    if (!out) return;

    std::uint64_t map_size = meetings.size();
    out.write(reinterpret_cast<const char*>(&map_size), sizeof(map_size));

    for (auto& [date, vec] : meetings)
    {
        out.write(reinterpret_cast<const char*>(&date), sizeof(date));

        std::uint64_t count = vec.size();
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (auto& m : vec)
            m.Serialize(out);
    }
}

void PI_Calendar::LoadMeetingsFromFile(std::string_view filename)
{
    std::ifstream in(filename.data(), std::ios::binary);
    if (!in) return;

    std::uint64_t map_size{};
    in.read(reinterpret_cast<char*>(&map_size), sizeof(map_size));

    for (std::uint64_t i = 0; i < map_size; ++i)
    {
        Date date{};
        in.read(reinterpret_cast<char*>(&date), sizeof(date));

        std::uint64_t count{};
        in.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (std::uint64_t j = 0; j < count; ++j)
            meetings[date].push_back(Meeting::Deserialize(in));
    }
}

void PI_Calendar::Meeting::Serialize(std::ostream& out) const
{
    std::uint64_t len = name.size();

    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(name.data(), static_cast<std::streamsize>(len));
}

PI_Calendar::Meeting PI_Calendar::Meeting::Deserialize(std::istream& in)
{
    Meeting m;

    std::uint64_t len{};
    in.read(reinterpret_cast<char*>(&len), sizeof(len));

    m.name.resize(len);
    in.read(m.name.data(), static_cast<std::streamsize>(len));

    return m;
}