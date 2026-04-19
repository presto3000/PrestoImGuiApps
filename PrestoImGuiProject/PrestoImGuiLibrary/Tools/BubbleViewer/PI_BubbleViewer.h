#pragma once
#include <vector>
#include <imgui.h>

class PI_BubbleViewer
{
public:
    void Init(int count = 50);
    void Update(float dt);          // phisics update, later
    void Draw(ImVec2 canvas_size);

private:
    
    struct Bubble
    {
        ImVec2 pos;
        ImVec2 vel;
        float radius;
        ImU32 color;
        float seed;
    };

    std::vector<Bubble> bubbles;
};