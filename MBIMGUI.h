#pragma once
#include <string>
#include <vector>
#include <map>

// Include everything needed for clients
#include "imgui.h"
#include "imfilebrowser.h"
#include "implot.h"

#include "MBIWindow.h"
#include "MBILogger.h"

class Renderer;

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

    enum _MBIConfigFlags
    {
        MBIConfig_displayLogWindow = 1 << 1,
        MBIConfig_displayMetrics = 1 << 2,
        MBIConfig_displayImGuiDemo = 1 << 3,
        MBIConfig_displayImPlotDemo = 1 << 4,
        MBIConfig_displayMenuBar = 1 << 5
    };
    typedef int MBIConfigFlags;

private:
    Renderer *m_pRenderer;
    std::string m_name;
    ImGuiWindowFlags m_windowFlags;
    std::map<MBIDockOption, MBIWindow *> m_windows; // TODO multiple maps ?

    MBIConfigFlags m_confFlags;
    static MBILogger m_logger;

    void SetupDockspace() const;

public:
    MBIMGUI(const std::string name, int width, int height, const MBIConfigFlags flags = 0);
    ~MBIMGUI();
    bool Init(float fontsize = 13.0f) const;
    void AddWindow(MBIWindow *window, MBIDockOption option = DOCK_NONE);
    void Show();
    void SetWindowFlags(const ImGuiWindowFlags flags);
    static MBILogger &GetLogger();
};