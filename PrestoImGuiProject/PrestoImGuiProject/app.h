#pragma once

#include <Tools/FileViewer/PI_FileBrowserView.h>
#include <Tools/PlotterViewer/PI_PlotterView.h>
#include <Tools/BubbleViewer/PI_BubbleViewer.h>
#include <Tools/TextEditorViewer/PI_TextEditorViewer.h>
#include <Tools/FileDiffViewer/PI_FileDiffViewer.h>
#include <Tools/PaintViewer/PI_PaintViewer.h>
#include <Tools/Calendar/PI_Calendar.h>


class App
{
public:
    enum class Mode { Files, Plotter, Bubbles, TextEditor, FileDiffViewer, Paint, Calendar };

    Mode mode = Mode::Files;

    PI_FileBrowserView filesView;
    PI_PlotterView plotter;
    PI_BubbleViewer bubbleViewer;
    PI_TextEditorViewer textEditor;
    PI_FileDiffViewer fileDiffViewer;
    PI_PaintViewer paintViewer;
	PI_Calendar calendar;

    void OnStart();
    void OnEnd();

    void Draw(float DeltaTime, float FPS);
    void DrawTopBar();
    void SetMode(Mode newMode);
};