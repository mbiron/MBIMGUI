
#pragma once
#include "MBIWindow.h"
#include "MBILogger.h"

namespace MBIMGUI
{
    /**
     * @brief Implements a logging windows displaying log events. This window received logs from the m_logger object.
     *
     */
    class MBILogWindow : public MBIWindow
    {
    public:
        /**
         * @brief Log windows mode
         *
         */
        typedef enum
        {
            MODE_WINDOW, ///< Displayed as a dockable window
            MODE_BAR     ///< Displayed as a bar at the bottom of the application main window
        } LOGWINDOW_MODE;

        /**
         * @brief Construct a new MBILogWindow object
         *
         * @param name Name of the window, will be displayed at the top
         * @param eMode Mode of the window, see @ref LOGWINDOW_MODE
         */
        explicit MBILogWindow(std::string_view name, LOGWINDOW_MODE eMode) noexcept : MBIWindow(name, 0, 0, MBIWindowConfig_hideableInMenu), m_mode(eMode)
        {
            if (m_mode == MODE_BAR)
            {
                m_imguiFlags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
            }
        }
        /**
         * @brief Display the window.
         *
         */
        void Display()
        {
            static constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;

            if (m_mode == MODE_BAR)
            {
                if (m_logger.m_logs.empty() == false)
                {
                    std::string log;
                    ImVec4 color;
                    /* Always show the latest error in priority */
                    if (m_logger.m_errToPopup.empty() == false)
                    {
                        log = m_logger.m_errToPopup;
                        color = GetLevelColor(LOG_LEVEL_ERROR);
                    }
                    else
                    {
                        /* Otherwise, display the latest log */
                        log = m_logger.m_logs.last().GetMessageLog();
                        color = GetLevelColor(m_logger.m_logs.last().GetLevel());
                    }
                    ImGui::ColorButton("#LevelColor", color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip, ImVec2(15, 15));
                    ImGui::SameLine();
                    ImGui::Text(log.c_str());
                }
            }
            else
            {
                /* Show logs in a table */
                if (ImGui::BeginTable("##logTable", 3, flags))
                {
                    /* Submit columns name */
                    ImGui::TableSetupColumn(ICON_FA_BUG " Level");
                    ImGui::TableSetupColumn(ICON_FA_BOOK_OPEN " Message", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn(ICON_FA_CALENDAR " Date");
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
                        ImGui::Selectable(log.GetMessageLog().c_str());
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
            }
            /* If popup has been activated */
            if (m_logger.m_popupOnError)
            {
                if (m_logger.m_displayPopup)
                {
                    /* Open the popup */
                    ImGui::OpenPopup("ERROR##popup");
                    m_logger.m_displayPopup = false;
                }
                /* Prepare popup for errors */
                if (ImGui::BeginPopupModal("ERROR##popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text(m_logger.m_errToPopup.c_str());
                    ImGui::Separator();
                    if (ImGui::Button("OK", ImVec2(120, 0)))
                    {
                        m_logger.m_errToPopup.clear();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
        }

    private:
        /**
         * @brief Convert the log level to the appropiate color object to be displayed by ImGui
         *
         * @param eLevel Level of the log
         * @return const ImVec4 Color corresponding to the log level
         */
        const ImVec4 inline GetLevelColor(MBILogLevel eLevel) const noexcept
        {
            switch (eLevel)
            {
            case LOG_LEVEL_WARNING:
                return ImVec4(255, 200, 0, 255); // Yellow
            case LOG_LEVEL_ERROR:
                return ImVec4(255, 0, 0, 255); // Red
            case LOG_LEVEL_DEBUG:
                return ImVec4(0, 100, 255, 255); // Blue
            default:
            case LOG_LEVEL_INFO:
                return ImVec4(0, 150, 0, 255); // Green
            }
        }

        LOGWINDOW_MODE m_mode; ///< Store the current mode of the window
    };
}