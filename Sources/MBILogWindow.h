
#pragma once
#include "MBIWindow.h"
#include "MBILogger.h"

class MBILogWindow : public MBIWindow
{
private:
    MBILogger &m_logger;
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
    MBILogWindow(std::string name, MBILogger &logger) : MBIWindow(name, 600, 800, MBIWindowConfig_hideableInMenu), m_logger(logger) {}
    void Display()
    {
        static const ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;

        if (ImGui::BeginTable("##logTable", 3, flags))
        {
            // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
            ImGui::TableSetupColumn("Level");
            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Date");
            ImGui::TableHeadersRow();
            for (auto log : m_logger.m_logs)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextColored(GetLevelColor(log.GetLevel()), log.GetLevelString().c_str());
                ImGui::TableNextColumn();
                ImGui::Text(log.GetMessageLog().c_str());
                ImGui::TableNextColumn();
                ImGui::Text(log.GetTime().c_str());
            }
            ImGui::EndTable();

            // Auto scroll down
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }

        if (m_logger.m_popupOnError)
        {
            static MBILogger::MBILog errorLog;

            if (m_logger.m_displayPopup)
            {
                // For now, assume that there is no race condition and the last log is the error one
                errorLog = m_logger.m_logs.last();
                ImGui::OpenPopup("ERROR##popup");
                m_logger.m_displayPopup = false;
            }
            // Popup for errors
            if (ImGui::BeginPopupModal("ERROR##popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(errorLog.GetMessageLog().c_str());
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