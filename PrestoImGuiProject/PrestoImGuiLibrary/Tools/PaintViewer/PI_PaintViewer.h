#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <imgui.h>

namespace fs = std::filesystem;

/**
 * @brief A simple paint application viewer built with Dear ImGui.
 *
 * Manages a canvas where the user can draw colored circles,
 * choose colors and brush sizes, and save/load drawings to binary files.
 */
class PI_PaintViewer
{
public:
    /// A single drawn point: position, color, and brush size
    using PointData = std::tuple<ImVec2, ImColor, float>;

    // --- Popup window configuration constants ---

    /// Flags to make popups static (no resize, move, collapse, or scroll)
    static constexpr auto popUpFlags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    /// Size of the save/load popup dialogs
    static constexpr auto popUpSize = ImVec2(300.0F, 100.0F);

    /// Uniform size for buttons inside popups
    static constexpr auto popUpButtonSize = ImVec2(120.0F, 0.0F);

    /// Position of the popup, centered in the 1280x720 window
    static constexpr auto popUpPos = ImVec2(1280.0F / 2.0F - popUpSize.x / 2.0F,
        720.0F / 2.0F - popUpSize.y / 2.0F);

public:
    /**
     * @brief Constructs the PI_PaintViewer with default state.
     *
     * Initializes an empty canvas, white draw color, brush size of 2,
     * and a default filename buffer.
     */
    PI_PaintViewer()
        : points({}), canvasPos({}), currentDrawColor(ImColor(255, 255, 255)),
        pointDrawSize(2.0F), filenameBuffer("test.bin") {
    };

    /// Renders the entire PI_PaintViewer window (menu + canvas)
    void Draw();

private:
    // --- UI draw methods ---

    /// Renders the top menu bar with Save, Load, Clear, color, and size controls
    void DrawMenu();

    /// Renders the interactive drawing canvas and handles mouse input
    void DrawCanvas();

    /// Renders the preset color buttons (Red, Green, Blue, White, Custom)
    void DrawColorButtons();

    /// Renders the brush size slider
    void DrawSizeSettings();

    /// Renders the Save modal popup with a filename input
    void DrawSavePopup();

    /// Renders the Load modal popup with a filename input
    void DrawLoadPopup();

    // --- File I/O ---

    /// Serializes all drawn points to a binary file at the given path
    void SaveToImageFile(std::string_view filename);

    /// Deserializes drawn points from a binary file and appends them to the canvas
    void LoadFromImageFile(std::string_view filename);

    /// Removes all drawn points, effectively clearing the canvas
    void ClearCanvas();

private:
    // --- Canvas dimensions ---

    std::uint32_t numRows = 800;     ///< Width of the canvas in pixels
    std::uint32_t numCols = 600;     ///< Height of the canvas in pixels

    /// Number of color channels (0 = Red, 1 = Green, 2 = Blue)
    std::uint32_t numChannels = 3;

    /// Canvas size as an ImVec2, derived from numRows x numCols
    ImVec2 canvasSize =
        ImVec2(static_cast<float>(numRows), static_cast<float>(numCols));

    // --- Drawing state ---

    /// All points drawn on the canvas, in draw order
    std::vector<PointData> points;

    /// Top-left position of the canvas in window-local coordinates (updated each frame)
    ImVec2 canvasPos;

    /// The currently active drawing color
    ImColor currentDrawColor;

    /// The radius of circles drawn on the canvas
    float pointDrawSize;

    /// Buffer holding the filename used for save/load operations
    char filenameBuffer[256];
};