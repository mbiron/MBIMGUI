// MBI : This widget is based on idbrii's work from https://github.com/ocornut/imgui/issues/1658#issuecomment-1086193100
// with modifications proposed by ambrosiogabe.

// My modified ComboWithFilter with fts_fuzzy_match as include.
//
// Adds arrow navigation, Enter to confirm, max_height_in_items, and fixed
// focus on open and avoids drawing past window edges.
// My contributions are CC0/public domain.

// Posted in issue: https://github.com/ocornut/imgui/issues/1658#issuecomment-1086193100

#include <vector>
#include <string>
#include <algorithm>

// Built using imgui v1.78 WIP
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

// https://github.com/forrestthewoods/lib_fts/blob/632ca1ea82bdf65688241bb8788c77cb242fba4f/code/fts_fuzzy_match.h
#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

// bgfx packs kenney image font with its imgui and this is a magnifying glass.
static const char *ICON_FA_SEARCH = u8"\ue935";

namespace ImGui
{

    static bool sortbysec_desc(const std::pair<int, int> &a, const std::pair<int, int> &b)
    {
        return (b.second < a.second);
    }

    static int index_of_key(
        std::vector<std::pair<int, int>> pair_list,
        int key)
    {
        for (int i = 0; i < pair_list.size(); ++i)
        {
            auto &p = pair_list[i];
            if (p.first == key)
            {
                return i;
            }
        }
        return -1;
    }

    // Copied from imgui_widgets.cpp
    static float CalcMaxPopupHeightFromItemCount(int items_count)
    {
        ImGuiContext &g = *GImGui;
        if (items_count <= 0)
            return FLT_MAX;
        return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
    }

    bool ComboWithFilter(const char *label, int *current_item, const std::vector<std::string> &items, int popup_max_height_in_items /*= -1 */)
    {
        using namespace fts;

        ImGuiContext &g = *GImGui;

        ImGuiWindow *window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        const ImGuiStyle &style = g.Style;

        int items_count = static_cast<int>(items.size());

        // Use imgui Items_ getters to support more input formats.
        const char *preview_value = NULL;
        if (*current_item >= 0 && *current_item < items_count)
            preview_value = items[*current_item].c_str();

        static int focus_idx = -1;
        static char pattern_buffer[256] = {0};

        bool value_changed = false;

        const ImGuiID id = window->GetID(label);
        static bool is_already_open = IsPopupOpen(id, ImGuiPopupFlags_None);
        const bool is_filtering = is_already_open && pattern_buffer[0] != '\0';

        int show_count = items_count;

        std::vector<std::pair<int, int>> itemScoreVector;
        if (is_filtering)
        {
            // Filter before opening to ensure we show the correct size window.
            // We won't get in here unless the popup is open.
            for (int i = 0; i < items_count; i++)
            {
                int score = 0;
                bool matched = fuzzy_match(pattern_buffer, items[i].c_str(), score);
                if (matched)
                    itemScoreVector.push_back(std::make_pair(i, score));
            }
            std::sort(itemScoreVector.begin(), itemScoreVector.end(), sortbysec_desc);
            int current_score_idx = index_of_key(itemScoreVector, focus_idx);
            if (current_score_idx < 0 && !itemScoreVector.empty())
            {
                focus_idx = itemScoreVector[0].first;
            }
            show_count = static_cast<int>(itemScoreVector.size());
        }

        // Define the height to ensure our size calculation is valid.
        if (popup_max_height_in_items == -1)
        {
            popup_max_height_in_items = 5;
        }
        popup_max_height_in_items = ImMin(popup_max_height_in_items, show_count);

        if (!(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        {
            int items = popup_max_height_in_items + 2; // extra for search bar
            SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(items)));
        }

        if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
            return false;

        if (!is_already_open)
        {
            focus_idx = *current_item;
            memset(pattern_buffer, 0, IM_ARRAYSIZE(pattern_buffer));
        }

        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(240, 240, 240, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0, 255));
        ImGui::PushItemWidth(-FLT_MIN);
        // Filter input
        if (!is_already_open)
        {
            ImGui::SetKeyboardFocusHere();
            is_already_open = true;
        }
        InputText("##ComboWithFilter_inputText", pattern_buffer, 256);

        const ImVec2 label_size = CalcTextSize(ICON_FA_SEARCH, NULL, true);
        const ImVec2 search_icon_pos(
            ImGui::GetItemRectMax().x - label_size.x - style.ItemInnerSpacing.x * 2,
            window->DC.CursorPos.y + style.FramePadding.y + g.FontSize * 0.3f);
        RenderText(search_icon_pos, ICON_FA_SEARCH);

        ImGui::PopStyleColor(2);

        int move_delta = 0;
        if (IsKeyPressedMap(ImGuiKey_UpArrow))
        {
            --move_delta;
        }
        else if (IsKeyPressedMap(ImGuiKey_DownArrow))
        {
            ++move_delta;
        }

        if (move_delta != 0)
        {
            if (is_filtering)
            {
                int current_score_idx = index_of_key(itemScoreVector, focus_idx);
                if (current_score_idx >= 0)
                {
                    const int count = (int)itemScoreVector.size();
                    current_score_idx = (current_score_idx + move_delta + count) % count;
                    focus_idx = itemScoreVector[current_score_idx].first;
                }
            }
            else
            {
                focus_idx = (focus_idx + move_delta + items_count) % items_count;
            }
        }

        ImVec2 list_box_header_size;
        list_box_header_size.x = 0.0f;
        list_box_header_size.y = ImGui::GetTextLineHeightWithSpacing() * popup_max_height_in_items + g.Style.FramePadding.y * 2.0f;
        if (ImGui::ListBoxHeader("##ComboWithFilter_itemList", list_box_header_size))
        {
            for (int i = 0; i < show_count; i++)
            {
                int idx = is_filtering ? itemScoreVector[i].first : i;
                PushID((void *)(intptr_t)idx);
                const bool item_selected = (idx == focus_idx);
                const char *item_text = items[idx].c_str();
                if (Selectable(item_text, item_selected))
                {
                    value_changed = true;
                    *current_item = idx;
                    CloseCurrentPopup();
                }

                if (item_selected)
                {
                    SetItemDefaultFocus();
                    // SetItemDefaultFocus doesn't work so also check IsWindowAppearing.
                    if (move_delta != 0 || IsWindowAppearing())
                    {
                        SetScrollHereY();
                    }
                }
                PopID();
            }
            ImGui::ListBoxFooter();

            if (IsKeyPressedMap(ImGuiKey_Enter))
            {
                value_changed = true;
                *current_item = focus_idx;
                CloseCurrentPopup();
            }
        }
        ImGui::PopItemWidth();
        ImGui::EndCombo();

        if (value_changed)
        {
            is_already_open = false;

            // Not sure if this is actually necessary
            ImGui::MarkItemEdited(g.CurrentWindow->DC.NavFocusScopeIdCurrent);
        }

        return value_changed;
    }

} // namespace ImGui