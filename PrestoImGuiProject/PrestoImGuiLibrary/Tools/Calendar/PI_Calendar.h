#pragma once

#include <array>
#include <chrono>
#include <compare>
#include <cstdint>
#include <map>
#include <string_view>
#include <vector>

#include <imgui.h>

class PI_Calendar
{
public:
    using Date = std::chrono::sys_days;

    struct Meeting
    {
        std::string name;

        void Serialize(std::ostream& out) const;
        static Meeting Deserialize(std::istream& in);

        bool operator==(const Meeting&) const = default;
    };

public:
    PI_Calendar();

    void Draw(std::string_view label);

    void LoadMeetingsFromFile(std::string_view filename);
    void SaveMeetingsToFile(std::string_view filename);

private:
    void DrawCalendar();
    void DrawDateCombo();
    void DrawAddMeetingWindow();
    void DrawMeetingList();

private:
    std::chrono::year_month_day selectedDate;

    bool addMeetingWindowOpen = false;
    float calendarFontSize = 5.0f;

    std::map<Date, std::vector<Meeting>> meetings;

    static constexpr ImVec2 windowSize = { 1280.0f, 720.0f };
    static constexpr ImVec2 windowPos = { 0.0f, 0.0f };
    static constexpr ImVec2 popupSize = { 300.0f, 100.0f };

    static constexpr std::array<std::string_view, 12> monthNames{
        "January","February","March","April",
        "May","June","July","August",
        "September","October","November","December"
    };

    static constexpr std::uint32_t minYear = 2000;
    static constexpr std::uint32_t maxYear = 2100;

    static constexpr ImGuiWindowFlags meetingWindowFlags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar;
};

