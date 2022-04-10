#pragma once
#include <Windows.h>
#include <string>
#include "Renderer.h"

class Win32Renderer : public Renderer
{
    private:
        Renderer * m_pRenderer;
        HWND m_hwnd;
        WNDCLASSEX m_wc;
        POINTS m_Pos;
        int m_heigt;
		int m_width;
      
    public:
        Win32Renderer(std::string name, int width, int height);
        ~Win32Renderer();
        bool Init();
        void NewFrame();
        void Shutdown();
        void Render();
        void Resize(void * param);
        void Destroy();
        HWND getWindowHandle();
        LRESULT Win32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};