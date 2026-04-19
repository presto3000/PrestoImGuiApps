#include "app.h"
#include <imgui.h>


void App::Draw(float DeltaTime, float FPS)
{
    ImGui::Begin("Debug");
    ImGui::Text("FPS: %.1f", FPS);
    ImGui::Text("DT: %.3f ms", DeltaTime * 1000.0f);
    ImGui::End();

    DrawTopBar(); // mode switching

    switch (mode)
    {
    case Mode::Files:
        filesView.Draw();
        break;
    }
}

void App::DrawTopBar()
{
    if (ImGui::Button("File Viewer"))
        SetMode(Mode::Files);
}

void App::SetMode(Mode newMode)
{
    mode = newMode;

}