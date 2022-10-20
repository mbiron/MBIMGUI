/* MBI : Toggle from nerdtronik, slightly modified by me.
https://github.com/ocornut/imgui/issues/1537#issuecomment-780262461
*/

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{

    bool ToggleButton(const char *str_id, bool *v)
    {
        bool bclicked = false;
        const ImVec4 *colors = ImGui::GetStyle().Colors;
        const ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        // MBI : Add displayable label
        const ImVec2 label_size = CalcTextSize(str_id, NULL, true);
        const float height = ImGui::GetFrameHeight();
        const float width = height * 1.55f;
        const float radius = height * 0.50f;

        ImGui::InvisibleButton(str_id, ImVec2(width, height));
        if (ImGui::IsItemClicked())
        {
            *v = !*v;
            bclicked = true;
        }
        ImGuiContext &gg = *GImGui;
        float ANIM_SPEED = 0.085f;
        if (gg.LastActiveId == gg.CurrentWindow->GetID(str_id)) // && g.LastActiveIdTimer < ANIM_SPEED)
            float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
        if (ImGui::IsItemHovered())
            draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : ImVec4(0.78f, 0.78f, 0.78f, 1.0f)), height * 0.5f);
        else
            draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_Button] : ImVec4(0.85f, 0.85f, 0.85f, 1.0f)), height * 0.50f);
        draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));

        if (label_size.x != 0.0)
        {
            ImGui::SameLine();
            ImGui::Text(str_id);
        }
        return bclicked;
    }
}