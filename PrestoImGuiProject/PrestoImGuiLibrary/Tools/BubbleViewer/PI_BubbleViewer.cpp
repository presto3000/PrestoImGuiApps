#include "PI_BubbleViewer.h"

static ImU32 HSVtoRGB(float h, float s, float v, int a)
{
    float r, g, b;

    int i = (int)(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    switch (i % 6)
    {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
    }

    return IM_COL32(
        (int)(r * 255),
        (int)(g * 255),
        (int)(b * 255),
        a
    );
}

void PI_BubbleViewer::Init(int count)
{
    bubbles.clear();
    bubbles.reserve(count);

    constexpr float fixedRadius = 32.0f;

    for (int i = 0; i < count; i++)
    {
        Bubble b;

        b.pos = ImVec2(rand() % 800, rand() % 600);

        b.vel = ImVec2(
            (rand() % 100 - 50) / 50.0f,
            (rand() % 100 - 50) / 50.0f
        );

        // same size for all bubbles
        b.radius = fixedRadius;

        // random color per bubble
        b.color = IM_COL32(
            rand() % 255,
            rand() % 255,
            rand() % 255,
            200
        );
        b.seed = (float)rand() / (float)RAND_MAX;

        bubbles.push_back(b);
    }
}

void PI_BubbleViewer::Draw(ImVec2 canvas_size)
{
    ImGui::Begin("Bubbles");

    ImDrawList* draw = ImGui::GetWindowDrawList();

    // REAL window rectangle
    ImVec2 winPos = ImGui::GetWindowPos();
    ImVec2 winSize = ImGui::GetWindowSize();

    ImVec2 minBound = winPos;
    ImVec2 maxBound = ImVec2(winPos.x + winSize.x,
        winPos.y + winSize.y);


    for (auto& b : bubbles)
    {
        // LEFT / RIGHT
        if (b.pos.x - b.radius < minBound.x)
        {
            b.pos.x = minBound.x + b.radius;
            b.vel.x *= -1;
        }
        else if (b.pos.x + b.radius > maxBound.x)
        {
            b.pos.x = maxBound.x - b.radius;
            b.vel.x *= -1;
        }

        // TOP / BOTTOM
        if (b.pos.y - b.radius < minBound.y)
        {
            b.pos.y = minBound.y + b.radius;
            b.vel.y *= -1;
        }
        else if (b.pos.y + b.radius > maxBound.y)
        {
            b.pos.y = maxBound.y - b.radius;
            b.vel.y *= -1;
        }

        // -------------------------
        // SOAP BUBBLE COLOR
        // -------------------------
        float t = (float)ImGui::GetTime();

        // stable identity (never changes with movement)
        float base = b.seed * 10.0f;

        // VERY slow drift
        float timeShift = t * 0.1f;

        // tiny shimmer only
        float shimmer = 0.02f * sinf(t * 0.5f + base);

        // final hue (stable + slow)
        float hue = fmodf(base * 0.1f + timeShift + shimmer, 1.0f);

        ImU32 coreColor = HSVtoRGB(
            hue,
            0.55f,
            1.0f,
            90
        );

        // -------------------------
        // GLOW
        // -------------------------
        draw->AddCircleFilled(
            b.pos,
            b.radius * 1.15f,
            IM_COL32(200, 220, 255, 30)
        );

        // -------------------------
        // MAIN BUBBLE
        // -------------------------
        draw->AddCircleFilled(
            b.pos,
            b.radius,
            coreColor
        );

        // -------------------------
        // HIGHLIGHT
        // -------------------------
        ImVec2 highlight = ImVec2(
            b.pos.x - b.radius * 0.3f,
            b.pos.y - b.radius * 0.3f
        );

        draw->AddCircleFilled(
            highlight,
            b.radius * 0.25f,
            IM_COL32(255, 255, 255, 140)
        );
    }

    ImGui::End();
}

void PI_BubbleViewer::Update(float dt)
{
    constexpr float speed = 120.0f;

    for (auto& b : bubbles)
    {
        b.pos.x += b.vel.x * speed * dt;
        b.pos.y += b.vel.y * speed * dt;
    }
}