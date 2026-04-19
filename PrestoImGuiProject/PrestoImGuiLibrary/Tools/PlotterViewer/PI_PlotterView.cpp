
#include "PI_PlotterView.h"
#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

void PI_PlotterView::Draw()
{
    ImGui::Begin("PlotterView");

    DrawSelection();
    DrawPlot();

    ImGui::End();
}

void PI_PlotterView::DrawSelection()
{
    if (ImGui::RadioButton("Line", plotType == EPlotType::Line))
        plotType = EPlotType::Line;

    ImGui::SameLine();

    if (ImGui::RadioButton("Scatter", plotType == EPlotType::Scatter))
        plotType = EPlotType::Scatter;

    ImGui::Separator();

    for (const auto func_name : functionNames)
    {
        const auto curr_function = functionNameMapping(func_name);
        bool selected = selectedFunctions.contains(curr_function);

        if (ImGui::Checkbox(func_name.data(), &selected))
        {
            if (selected)
                selectedFunctions.insert(curr_function);
            else
                selectedFunctions.erase(curr_function);
        }
    }
}

void PI_PlotterView::DrawPlot()
{
    static constexpr auto num_points = 1000;
    static constexpr auto x_min = -100.0;
    static constexpr auto x_max = 100.0;
    static const auto x_step = (x_max - x_min) / num_points;

    static auto xs = std::array<double, num_points>{};
    static auto ys = std::array<double, num_points>{};

    if (selectedFunctions.size() == 0 ||
        selectedFunctions.size() == 1 && selectedFunctions.contains(EFunction::Unknown))
    {
        if (ImPlot::BeginPlot("###plot", ImVec2(-1.f, -1.f), ImPlotFlags_NoTitle)) {
            ImPlot::EndPlot();
        }
        return;
    }

    if (ImPlot::BeginPlot("###plot", ImVec2(-1.f, -1.f), ImPlotFlags_NoTitle))
    {
        for (const auto& function : selectedFunctions)
        {
            if (function == EFunction::Unknown)
                continue;

            auto x = x_min;
            for (std::size_t i = 0; i < num_points; ++i)
            {
                xs[i] = x;
                ys[i] = evaluateFunction(function, x);
                x += x_step;
            }

            const auto plot_label = fmt::format("{}###{}",
                functionNames[static_cast<int>(function)],
                static_cast<int>(function));

            if (plotType == EPlotType::Line)
                ImPlot::PlotLine(plot_label.c_str(), xs.data(), ys.data(), num_points);
            else
                ImPlot::PlotScatter(plot_label.c_str(), xs.data(), ys.data(), num_points);
        }

        ImPlot::EndPlot();
    }


}

PI_PlotterView::EFunction PI_PlotterView::functionNameMapping(std::string_view function_name)
{
    if (std::string_view{ "Sine Wave" } == function_name)
        return PI_PlotterView::EFunction::Sine;
    else if (std::string_view{ "Cosine Wave" } == function_name)
        return PI_PlotterView::EFunction::Cosine;
    else if (std::string_view{ "Tangent Wave" } == function_name)
        return PI_PlotterView::EFunction::Tangent;
    else
        return PI_PlotterView::EFunction::Unknown;
}


double PI_PlotterView::evaluateFunction(const EFunction function, const double x)
{
    switch (function)
    {
    case EFunction::Sine:
        return std::sin(x);
    case EFunction::Cosine:
        return std::cos(x);
    case EFunction::Tangent:
        return std::tan(x);
    default:
        return 0.0;
    }
}
