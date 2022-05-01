
#pragma once
#include "MBIWindow.h"
#include "MBILogger.h"


class MBILogWindow : public MBIWindow
{
private:
    MBILogger& m_logger;

public:
    MBILogWindow(std::string name, MBILogger& logger) : MBIWindow(name, 600, 800) , m_logger(logger){}
    void Display()
    {
        ImGui::BeginTable("##logTable", 3);
        for(auto log : m_logger.m_logs)
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

    MBILogger& GetLogger(){return m_logger;}

};