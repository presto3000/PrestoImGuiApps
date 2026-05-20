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

    if (state.showFiles)
    {
        filesView.Draw(state.showFiles);
    }

    if (state.showPlotter)
    {
        plotter.Draw(state.showPlotter);
    }

    if (state.showBubbles)
    {
        ImVec2 size = ImGui::GetMainViewport()->Size;
        bubbleViewer.Update(DeltaTime);
        bubbleViewer.Draw(size, state.showBubbles);
    }

    if (state.showTextEditor)
    {
        textEditor.Draw(state.showTextEditor);
    }

    if (state.showFileDiff)
    {
        fileDiffViewer.Draw(state.showFileDiff);
    }

    if (state.showPaint)
    {
        paintViewer.Draw(state.showPaint);
    }

    if (state.showCalendar)
    {
        calendar.Draw("Presto Calendar", state.showCalendar);
    }

    if (state.showCSVEditor)
    {
        csvEditor.Draw("CSV Editor", state.showCSVEditor);
    }

    if (state.showClockViewer)
    {
        clockViewer.Draw("Clock Viewer", state.showClockViewer);
    }
}

void App::DrawTopBar()
{
    if (ImGui::Button("Files"))
        state.showFiles = true;

    ImGui::SameLine();

    if (ImGui::Button("Plotter"))
        state.showPlotter = true;

    ImGui::SameLine();

    if (ImGui::Button("Bubbles"))
        if (!state.showBubbles)
        {
            state.showBubbles = true;
            bubbleViewer.Init(100);
        }


    ImGui::SameLine();

    if (ImGui::Button("Text Editor"))
        state.showTextEditor = true;

    ImGui::SameLine();

    if (ImGui::Button("Paint"))
        state.showPaint = true;

    ImGui::SameLine();

    if (ImGui::Button("Calendar"))
        state.showCalendar = true;

    ImGui::SameLine();

    if (ImGui::Button("CSV"))
        state.showCSVEditor = true;
    
    ImGui::SameLine();

    if (ImGui::Button("Clock Viewer"))
        state.showClockViewer = true;
}

void App::SetMode(Mode newMode)
{
    mode = newMode;
    if (mode == Mode::Bubbles)
        bubbleViewer.Init(100);
}