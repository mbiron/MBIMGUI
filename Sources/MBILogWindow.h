
#pragma once
#include "MBIWindow.h"
#include "MBILogger.h"
#include "MBICircularBuffer.h"

class MBILogWindow : public MBIWindow
{
private:
    MBILogger &m_logger;

public:
    MBILogWindow(std::string name, MBILogger &logger) : MBIWindow(name, 600, 800), m_logger(logger) {}
    void Display()
    {
        static const ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
      
        if (ImGui::BeginTable("##logTable", 3, flags))
        {
            // Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
            // (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
            ImGui::TableSetupColumn("Level");
            ImGui::TableSetupColumn("Message",ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Date"); 
            ImGui::TableHeadersRow();
            for (auto log : m_logger.m_logs)
            {
                ImGui::TableNextRow();
                 ImGui::TableNextColumn();
                ImGui::Text(log.GetLevel().c_str());
                 ImGui::TableNextColumn();
                ImGui::Text(log.GetMessage().c_str());
                 ImGui::TableNextColumn();
                ImGui::Text(log.GetTime().c_str());
            }

            ImGui::EndTable();
        }
    }
    MBILogger &GetLogger() { return m_logger; }
};