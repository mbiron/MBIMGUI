#pragma once
#include <Windows.h>
#include "Renderer.h"

#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD

#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include <xinput.h>
typedef DWORD (WINAPI *PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES*);
typedef DWORD (WINAPI *PFN_XInputGetState)(DWORD, XINPUT_STATE*);
#endif

 struct Win32_Data
    {
        HWND                        hWnd;
        HWND                        MouseHwnd;
        bool                        MouseTracked;
        int                         MouseButtonsDown;
        INT64                       Time;
        INT64                       TicksPerSecond;
        ImGuiMouseCursor            LastMouseCursor;
        bool                        HasGamepad;
        bool                        WantUpdateHasGamepad;

    #ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
        HMODULE                     XInputDLL;
        PFN_XInputGetCapabilities   XInputGetCapabilities;
        PFN_XInputGetState          XInputGetState;
    #endif

        Win32_Data()      { memset((void*)this, 0, sizeof(*this)); }
    };

class Win32Renderer : public Renderer
{
    private:
        Renderer * m_pRenderer;
        HWND m_hwnd;
        WNDCLASSEX m_wc;
        POINTS m_Pos;
        int window_size_x;

        Win32_Data* GetBackendData();
      
    public:
        Win32Renderer(int width, int height);
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