#pragma once

#include <Tools/FileViewer/PI_FileBrowserView.h>
#include <Tools/PlotterViewer/PI_PlotterView.h>
#include <Tools/BubbleViewer/PI_BubbleViewer.h>
#include <Tools/TextEditorViewer/PI_TextEditorViewer.h>
#include <Tools/FileDiffViewer/PI_FileDiffViewer.h>
#include <Tools/PaintViewer/PI_PaintViewer.h>
#include <Tools/Calendar/PI_Calendar.h>
#include <Tools/CSVEditor/PI_CSVEditor.h>
#include <Tools/ClockViewer/PI_ClockViewer.h>

struct AppState
{
    bool showFiles = false;
    bool showPlotter = false;
    bool showBubbles = false;
    bool showTextEditor = false;
    bool showFileDiff = false;
    bool showPaint = false;
    bool showCalendar = false;
    bool showCSVEditor = false;
    bool showClockViewer = false;
};

class App
{
public:
    enum class Mode { Files, Plotter, Bubbles, TextEditor,
        FileDiffViewer, Paint, Calendar , CSVEditor };

    Mode mode = Mode::Files;

    PI_FileBrowserView filesView;
    PI_PlotterView plotter;
    PI_BubbleViewer bubbleViewer;
    PI_TextEditorViewer textEditor;
    PI_FileDiffViewer fileDiffViewer;
    PI_PaintViewer paintViewer;
	PI_Calendar calendar;
	PI_CSVEditor csvEditor;
    PI_ClockViewer clockViewer;

    void OnStart();
    void OnEnd();

    void Draw(float DeltaTime, float FPS);
    void DrawTopBar();
    void SetMode(Mode newMode);

private:
    AppState state;
};