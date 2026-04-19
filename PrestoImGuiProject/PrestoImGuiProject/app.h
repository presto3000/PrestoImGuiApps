#pragma once

#include <Tools/FileViewer/PI_FileBrowserView.h>
#include <Tools/PlotterViewer/PI_PlotterView.h>


class App
{
public:
    enum class Mode { Files, Plotter };

    Mode mode = Mode::Files;

    PI_FileBrowserView filesView;
    PI_PlotterView plotter;

    void Draw(float DeltaTime, float FPS);
    void DrawTopBar();
    void SetMode(Mode newMode);
};