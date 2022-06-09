
#pragma once
#include "MBIWindow.h"
#include "MBILogger.h"

class MBILogWindow : public MBIWindow
{
private:
    const ImVec4 inline GetLevelColor(MBILogger::MBILogLevel eLevel) const
    {
        switch (eLevel)
        {
        case MBILogger::LOG_LEVEL_WARNING:
            return ImVec4(255, 200, 0, 255); // Yellow
        case MBILogger::LOG_LEVEL_ERROR:
            return ImVec4(255, 0, 0, 255); // Red
        case MBILogger::LOG_LEVEL_DEBUG:
            return ImVec4(0, 100, 255, 255); // Blue
        default:
        case MBILogger::LOG_LEVEL_INFO:
            return ImVec4(0, 150, 0, 255); // Green
        }
    }

public:
    MBILogWindow(std::string name) : MBIWindow(name, 0, 0, MBIWindowConfig_hideableInMenu) {}
    void Display()
    {
        static const ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;

        if (ImGui::BeginTable("##logTable", 3, flags))
        {
            /* Submit columns name */
            ImGui::TableSetupColumn("Level");
            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Date");
            ImGui::TableHeadersRow();
            for (const auto log : m_logger.m_logs)
            {
                /* Add a new row */
                ImGui::TableNextRow();
                /* Display log level */
                ImGui::TableNextColumn();
                ImGui::TextColored(GetLevelColor(log.GetLevel()), log.GetLevelString().c_str());
                /* Display log text */
                ImGui::TableNextColumn();
                ImGui::Text(log.GetMessageLog().c_str());
                if (ImGui::IsItemHovered())
                {
                    // Show tooltip if column is too tight to display full text
                    ImVec2 textSize = ImGui::CalcTextSize(log.GetMessageLog().c_str());
                    if (ImGui::GetColumnWidth() < textSize.x)
                    {
                        ImGui::SetTooltip(log.GetMessageLog().c_str());
                    }
                }
                /* Display log date */
                ImGui::TableNextColumn();
                ImGui::Text(log.GetTime().c_str());
            }
            ImGui::EndTable();

            /* Auto scroll down when adding new logs */
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }

        if (m_logger.m_popupOnError)
        {
            if (m_logger.m_displayPopup)
            {
                ImGui::OpenPopup("ERROR##popup");
                m_logger.m_displayPopup = false;
            }
            // Popup for errors
            if (ImGui::BeginPopupModal("ERROR##popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(m_logger.m_errToPopup.c_str());
                ImGui::Separator();
                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }
};