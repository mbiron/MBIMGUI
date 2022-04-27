#pragma once
#include <string>
#include <vector>
#include "imgui.h"
#include "implot.h"
#include "Renderer.h"
#include "MBIWindow.h"

class MBIMGUI
{
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
        Renderer * m_pRenderer;
		std::string m_name;
		ImGuiWindowFlags m_windowFlags;
        MBIWindow &m_window;
		std::vector<MBIWindow*> m_secWindows;
        ImGuiViewport *m_viewport;
        ImGuiID m_dockspaceId;

        void SetupDockspace();
		
    public:
        MBIMGUI(const std::string name, MBIWindow &window);
        ~MBIMGUI();
        bool Init();
		void AddChildWindow(MBIWindow *window, MBIDockOption option = DOCK_NONE);
        void Show();
		void SetWindowFlags(const ImGuiWindowFlags flags);
};