#include "app.h"
#include <imgui.h>


void App::OnStart()
{
	calendar.LoadMeetingsFromFile("meetings.bin");
}

void App::OnEnd()
{
    calendar.SaveMeetingsToFile("meetings.bin");
}

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
        case Mode::Plotter:
            plotter.Draw();
            break;
            ImGui::SameLine();
        case Mode::Bubbles:
        {
            ImVec2 size = ImGui::GetMainViewport()->Size;
            bubbleViewer.Update(DeltaTime);   // simulation step
            bubbleViewer.Draw(size);
            break;
        }
        case Mode::TextEditor:
            textEditor.Draw();
            break;
        case Mode::FileDiffViewer:
            fileDiffViewer.Draw();
			break;
		case Mode::Paint:
            paintViewer.Draw();
			break;
		case Mode::Calendar:
            calendar.Draw("Presto Calendar");
            break;

    }
}

void App::DrawTopBar()
{
    if (ImGui::Button("File Viewer"))
        SetMode(Mode::Files);

    ImGui::SameLine();

    if (ImGui::Button("Plotter"))
        SetMode(Mode::Plotter);

    ImGui::SameLine();

    if (ImGui::Button("Bubbles"))
        SetMode(Mode::Bubbles);

    ImGui::SameLine();

    if (ImGui::Button("Text Editor"))
        SetMode(Mode::TextEditor);

    ImGui::SameLine();

    if (ImGui::Button("File Diff Viewer"))
		SetMode(Mode::FileDiffViewer);

    ImGui::SameLine();

    if (ImGui::Button("Paint"))
		SetMode(Mode::Paint);

    ImGui::SameLine();

    if (ImGui::Button("Calendar"))
		SetMode(Mode::Calendar);

}

void App::SetMode(Mode newMode)
{
    mode = newMode;
    if (mode == Mode::Bubbles)
        bubbleViewer.Init(100);
}