#pragma once
#include <string>
#include <vector>
#include <map>
#include "imgui.h"
#include "implot.h"
#include "Renderer.h"
#include "MBIWindow.h"

class MBIMGUI
{
public:
    typedef enum _MBIDockOption
    {
        DOCK_NONE,
        DOCK_MAIN,
        DOCK_LEFT,
        DOCK_RIGHT,
        DOCK_UP,
        DOCK_DOWN
    } MBIDockOption;

private:
    Renderer *m_pRenderer;
    std::string m_name;
    ImGuiWindowFlags m_windowFlags;
    //std::vector<MBIWindow *> m_Windows;
    std::map<MBIDockOption,MBIWindow *> m_Windows; // TODO multiple maps ?
    ImGuiViewport *m_viewport;
    ImGuiID m_dockspaceId;

    void SetupDockspace();

public:
    MBIMGUI(const std::string name, int width, int height);
    ~MBIMGUI();
    bool Init();
    void AddWindow(MBIWindow *window, MBIDockOption option = DOCK_NONE);
    void Show();
    void SetWindowFlags(const ImGuiWindowFlags flags);
};