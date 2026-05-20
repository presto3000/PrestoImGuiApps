#include <chrono>
#include <cmath>
#include <string_view>
#include <tuple>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "PI_ClockViewer.h"

void PI_ClockViewer::Draw(std::string_view label, bool& open)
{
    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar;

    ImGui::SetNextWindowSize(ImVec2(1280, 720));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    if (!ImGui::Begin(label.data(), &open, flags))
    {
        ImGui::End();
        return;
    }

    const ImVec2 cursor = ImGui::GetCursorScreenPos();
    center = ImVec2(cursor.x + circleRadius,
        cursor.y + circleRadius);

    UpdateTime();

    float hAngle, mAngle, sAngle;
    ComputeAngles(hAngle, mAngle, sAngle);

    DrawFace();

    DrawHand(circleRadius * HOUR_HAND, hAngle, ImColor(255, 80, 80), 3.0f);
    DrawHand(circleRadius * MIN_HAND, mAngle, ImColor(80, 255, 80), 3.0f);
    DrawHand(circleRadius * SEC_HAND, sAngle, ImColor(80, 120, 255), 2.0f);

    DrawTicks();

    DrawDigital();

    ImGui::End();
}

void PI_ClockViewer::DrawFace()
{
    auto* dl = ImGui::GetWindowDrawList();

    // dl->AddCircle(center,
    //     circleRadius,
    //     ImGui::GetColorU32(ImGuiCol_Text),
    //     100,
    //     2.0f);
    // 
    // dl->AddCircle(center,
    //     innerRadius,
    //     ImGui::GetColorU32(ImGuiCol_Text),
    //     32,
    //     2.0f);

        // Outer bezel (slightly larger, darker feel)
    dl->AddCircleFilled(
        center,
        circleRadius * bezelRadiusFactor,
        ImGui::GetColorU32(ImGuiCol_FrameBg),
        128
    );

    // Main dial
    dl->AddCircleFilled(
        center,
        circleRadius,
        ImGui::GetColorU32(ImGuiCol_WindowBg),
        128
    );

    // Subtle outer ring
    dl->AddCircle(
        center,
        circleRadius,
        IM_COL32(255, 255, 255, 30),
        128,
        2.0f
    );

    // Center cap
    dl->AddCircleFilled(
        center,
        innerRadius * 2.0f,
        IM_COL32(220, 220, 220, 255)
    );
}

void PI_ClockViewer::DrawHand(float radius,
    float angle,
    ImColor color,
    float thickness)
{
    auto* dl = ImGui::GetWindowDrawList();

    float c = std::cos(angle);
    float s = std::sin(angle);

    ImVec2 end{
        center.x - radius * c,
        center.y - radius * s
    };

    // Glow pass (fat transparent stroke)
    dl->AddLine(
        center,
        end,
        IM_COL32(255, 255, 255, 40),
        handGlowThickness
    );

    // Main hand
    dl->AddLine(
        center,
        end,
        color,
        thickness
    );

    // Sharp highlight core (thin bright spine)
    dl->AddLine(
        center,
        end,
        IM_COL32(255, 255, 255, 180),
        1.0f
    );
}

void PI_ClockViewer::DrawTicks()
{
    auto* dl = ImGui::GetWindowDrawList();

    for (int i = 0; i < 60; ++i)
    {
        const float t = TAU * (float(i) / 60.0f) + offset;
        const float c = std::cos(t);
        const float s = std::sin(t);

        const bool hourMark = (i % 5 == 0);

        const float inner = hourMark
            ? circleRadius * HOUR_STROKE_INNER
            : circleRadius * MIN_STROKE_INNER;

        const float thickness = hourMark ? 2.0f : 1.0f;

        ImVec2 start{
            center.x + inner * c,
            center.y + inner * s
        };

        ImVec2 end{
            center.x + circleRadius * c,
            center.y + circleRadius * s
        };

        dl->AddLine(start, end,
            ImGui::GetColorU32(ImGuiCol_Text),
            thickness);
    }
}

void PI_ClockViewer::DrawDigital() const
{
    ImGui::SetCursorPos(ImVec2(20, 20));
    ImGui::Text("%02d:%02d:%02d", hrs, mins, secs);
}

void PI_ClockViewer::UpdateTime()
{
    const auto now = std::chrono::system_clock::now();
    const auto t = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};

#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm); // linux/macOS
#endif

    hrs = tm.tm_hour;
    mins = tm.tm_min;
    secs = tm.tm_sec;
}

void PI_ClockViewer::ComputeAngles(float& h, float& m, float& s) const
{
    const float sec = static_cast<float>(secs);
    const float min = static_cast<float>(mins) + sec / 60.0f;
    const float hr = static_cast<float>(hrs % 12) + min / 60.0f;

    h = hr * (TAU / 12.0f) + offset;
    m = min * (TAU / 60.0f) + offset;
    s = sec * (TAU / 60.0f) + offset;
}