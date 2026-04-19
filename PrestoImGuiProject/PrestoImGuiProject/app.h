#pragma once

#include <Tools/FileViewer/PI_FileBrowserView.h>


class App
{
public:
    enum class Mode { Files };

    Mode mode = Mode::Files;

    PI_FileBrowserView filesView;

    void Draw(float DeltaTime, float FPS);
    void DrawTopBar();
    void SetMode(Mode newMode);
};