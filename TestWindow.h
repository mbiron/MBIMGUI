#pragma once
#include "MBIWindow.h"
#include "ComPortMgr.h"

class TestWindow : public MBIWindow
{
private:
    // utility structure for realtime plot
    struct ScrollingBuffer
    {
        int MaxSize;
        int Offset;
        ImVector<ImVec2> Data;
        ScrollingBuffer(int max_size = 2000)
        {
            MaxSize = max_size;
            Offset = 0;
            Data.reserve(MaxSize);
        }
        void AddPoint(float x, float y)
        {
            if (Data.size() < MaxSize)
                Data.push_back(ImVec2(x, y));
            else
            {
                Data[Offset] = ImVec2(x, y);
                Offset = (Offset + 1) % MaxSize;
            }
        }
        void Erase()
        {
            if (Data.size() > 0)
            {
                Data.shrink(0);
                Offset = 0;
            }
        }
    };

public:
    TestWindow() : MBIWindow(600, 800) {}
    void Display()
    {
        static float f = 0.0f;
        static int counter = 0;
        static bool show_demo_window = false;
        static bool port_is_running = false;
        static ComPortMgr comport = ComPortMgr();

        std::vector<ComPortMgr::COMPORT> ports;
        static int item_current_idx = 0;
        comport.GetAvailableComPorts(ports);

        // Comport list
        if (ImGui::BeginCombo("combo 1", ports[item_current_idx].port))
        {
            for (int n = 0; n < ports.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(ports[n].port, is_selected))
                    item_current_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // Start/Stop Button
        ImGui::SameLine();
        if (!port_is_running)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 200, 0, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 230, 0, 255));
            if (ImGui::Button("Start"))
            {
                port_is_running = true;
                comport.StartCom(ports[item_current_idx]);
            }
            ImGui::PopStyleColor(2);
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 0, 0, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(230, 0, 0, 255));
            if (ImGui::Button("Stop"))
            {
                port_is_running = false;
            }
            ImGui::PopStyleColor(2);
        }

        // Graph
        if (port_is_running)
        {
            static bool pause = false;
            static ScrollingBuffer data;
            static float history = 10.0f;
            static float t = 0;

            if (!pause)
            {
                t += ImGui::GetIO().DeltaTime;
                // GetNextData
                data.AddPoint(t, comport.getNextData());
            }

            ImGui::Dummy(ImVec2(-1, 50));
            ImGui::SliderFloat("Time History", &history, 1, 30, "%.1f s");

            ImGui::Checkbox("Pause", &pause);

            if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 300)))
            {
                ImPlot::SetupAxes("Time", "Unit", ImPlotAxisFlags_NoMenus, 0);
                ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, (pause) ? ImGuiCond_Once : ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
                ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
                ImPlot::PlotLine("Data", &data.Data[0].x, &data.Data[0].y, data.Data.size(), data.Offset, 2 * sizeof(float));
                ImPlot::EndPlot();
            }
        }
    }
};