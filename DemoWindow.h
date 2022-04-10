#pragma once
#include "MBIWindow.h"

class DemoWindow : public MBIWindow
{
public:
    DemoWindow() : MBIWindow(400,400) {}
    void Display()
    {
        ImGui::ShowDemoWindow();
        ImPlot::ShowDemoWindow();
    }
};