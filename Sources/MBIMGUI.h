#pragma once
#include <string>
#include <vector>
#include "imgui.h"
#include "Renderer.h"
#include "MBIWindow.h"

class MBIMGUI
{
    private:
        Renderer * m_pRenderer;
		std::string m_name;
		ImGuiWindowFlags m_windowFlags;
        MBIWindow &m_window;
		std::vector<MBIWindow*> m_secWindows;

		
    public:
        MBIMGUI(const std::string name, MBIWindow &window);
        ~MBIMGUI();
        bool Init() const;
		void AddChildWindow(MBIWindow *window);
        void Show() const;
		void SetWindowFlags(const ImGuiWindowFlags flags);
};