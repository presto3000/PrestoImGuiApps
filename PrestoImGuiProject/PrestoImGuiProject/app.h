#pragma once

#include <Tools/FileViewer/PI_FileBrowserView.h>
#include <Tools/PlotterViewer/PI_PlotterView.h>
#include <Tools/BubbleViewer/PI_BubbleViewer.h>


class App
{
public:
    enum class Mode { Files, Plotter, Bubbles };

    Mode mode = Mode::Files;

    PI_FileBrowserView filesView;
    PI_PlotterView plotter;
    PI_BubbleViewer bubbleViewer;

    void Draw(float DeltaTime, float FPS);
    void DrawTopBar();
    void SetMode(Mode newMode);
};