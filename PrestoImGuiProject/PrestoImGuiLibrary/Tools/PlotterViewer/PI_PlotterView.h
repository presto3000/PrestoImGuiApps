#pragma once

#include <vector>
#include <string_view>
#include <array>
#include <set>

enum class EPlotType
{
    Line,
    Scatter
};

class PI_PlotterView
{
public:
    void Draw();

    constexpr static auto functionNames = std::array<std::string_view, 4>{
        "unknown",
        "Sine Wave",
        "Cosine Wave",
        "Tangent Wave"
    };
    enum class EFunction {
        Unknown = 0,
        Sine = 1,
        Cosine = 2,
        Tangent = 3
    };

    std::set<EFunction> selectedFunctions;

private:
    void DrawSelection();
    void DrawPlot();

    EFunction functionNameMapping(std::string_view function_name);
    double evaluateFunction(const EFunction function, const double x);


private:
    EPlotType plotType = EPlotType::Line;
};