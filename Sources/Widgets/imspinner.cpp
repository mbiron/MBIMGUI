#include <algorithm>
#include "imgui.h"
#include "imgui_internal.h"
#include "imspinner.h"

/* Just a wrapper around imspinner by Dalerank, see imspinner.h for more infos */
namespace ImGui
{
    void DoubleCircularSpinner(const char *label, const ImColor &color1 = 0xffffffff, const ImColor &color2 = 0xff0000ff)
    {
        ImSpinner::SpinnerTwinAng180(label, 8.0f, 6.0f, 4.0f, ImColor(255, 255, 255), ImColor(255, 0, 0), 4.0f);
    }

    void SimpleCircularSpinner(const char *label, const ImColor &color = 0xffffffff)
    {
        ImSpinner::SpinnerAng(label, 8.0f, 2.0f, color, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg), 8.0f);
    }

}