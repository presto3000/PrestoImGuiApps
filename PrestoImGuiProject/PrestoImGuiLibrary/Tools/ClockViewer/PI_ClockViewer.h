#pragma once

#include <cstdint>
#include <numbers>
#include <string_view>
#include <tuple>

#include <imgui.h>

class PI_ClockViewer
{
private:
    static constexpr float PI = std::numbers::pi_v<float>;

    static constexpr float circleRadius = 250.0f;
    static constexpr float offset = PI * 0.5f; // 12 o'clock alignment

    static constexpr float innerRadius = 5.0f;

    static constexpr float HOUR_HAND = 0.55f;
    static constexpr float MIN_HAND = 0.85f;
    static constexpr float SEC_HAND = 0.65f;

    static constexpr float HOUR_STROKE_INNER = 0.90f;
    static constexpr float MIN_STROKE_INNER = 0.95f;

    static constexpr float TAU = 2.0f * PI;

    static constexpr float bezelRadiusFactor = 1.02f;
    static constexpr float hourMarkerInner = 0.82f;
    static constexpr float minuteMarkerInner = 0.92f;

    static constexpr float handGlowThickness = 6.0f;

public:
    PI_ClockViewer() : secs(0), mins(0), hrs(0), center({}) {};

    void Draw(std::string_view label, bool& open);

private:
    void UpdateTime();
    void ComputeAngles(float& h, float& m, float& s) const;

    void DrawFace();
    void DrawHand(float radius, float angle, ImColor color, float thickness);
    void DrawTicks();

    void DrawDigital() const;


private:
    std::int32_t secs;
    std::int32_t mins;
    std::int32_t hrs;

    ImVec2 center{};
};