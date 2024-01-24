#include <algorithm>
#include "imgui.h"
#include "imgui_internal.h"
#include "imspinner.h"

/* Just a wrapper around imspinner by Dalerank, see imspinner.h for more infos */
namespace ImGui
{
    void DoubleCircularSpinner(const char *label, const ImColor &color1, const ImColor &color2)
    {
        ImSpinner::SpinnerTwinAng180(label, 8.0f, 6.0f, 4.0f, color1, color2, 4.0f);
    }

    void SimpleCircularSpinner(const char *label, const ImColor &color)
    {
        ImSpinner::SpinnerAng(label, 8.0f, 2.0f, color, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg), 8.0f);
    }

    void HeartSpinner(const char *label, const ImColor &color)
    {
        constexpr const float radius = 10.0f;
        constexpr const float half_radius = radius / 2.0f;
        /* Center heart on the line */
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - half_radius);
        ImSpinner::SpinnerRotatingHeart(label, radius, 1.5f, color, 8.0f);
    }
}