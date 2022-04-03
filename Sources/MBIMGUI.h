#pragma once
#include "imgui.h"
#include "Renderer.h"

class MBIMGUI
{
    typedef void (*MBIMGUI_Callback)(void *);

    private:
        int m_width;
        int m_height;
        Renderer * m_pRenderer;
        MBIMGUI_Callback m_cb;
        void * m_cbArg;
    public:
        MBIMGUI(MBIMGUI_Callback cb, void * arg, int width = 200, int height = 200);
        ~MBIMGUI();
        bool Init();
        void Display();
};