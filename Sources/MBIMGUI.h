#pragma once
#include <string>
#include "imgui.h"
#include "Renderer.h"
//#include "MBIWindow.h"

class MBIMGUI
{
    typedef void (*MBIMGUI_Callback)(void *);

    private:
        int m_width;
        int m_height;
        Renderer * m_pRenderer;
        MBIMGUI_Callback m_cb;
        void * m_cbArg;
		std::string m_name;
		ImGuiWindowFlags m_windowFlags;
		//std::vector<MBIWindow> m_windows;
		
    public:
        MBIMGUI(std::string name, MBIMGUI_Callback cb, void * arg = NULL, int width = 200, int height = 200);
        ~MBIMGUI();
        bool Init();
		//void AddWindow(MBIWindow &window)
        void Display();
		void SetWindowFlags(ImGuiWindowFlags flags);
};