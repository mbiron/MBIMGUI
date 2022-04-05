#include <tchar.h>
#include <windowsx.h>
#include "Imgui.h"
#include "Win32Renderer.h"
#include "Dx12Renderer.h"
#include "imgui_impl_win32.h"


#if 1
#define MBIMGUI_WINDOW_STYLE (WS_POPUP | WS_EX_TOOLWINDOW)
#else
#define MBIMGUI_WINDOW_STYLE (WS_POPUP | WS_THICKFRAME |  WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX)
#endif 


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Win32Renderer *pThis; // our "this" pointer will go here
    if (msg == WM_NCCREATE)
    {
        // Recover the "this" pointer which was passed as a parameter
        // to CreateWindow(Ex).
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<Win32Renderer *>(lpcs->lpCreateParams);
        // Put the value in a safe place for future use
        SetWindowLongPtr(hWnd, GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        // Recover the "this" pointer from where our WM_NCCREATE handler
        // stashed it.
        pThis = reinterpret_cast<Win32Renderer *>(
            GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            pThis->Resize((void *)lParam);           
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


/**
 * @brief Construct a new Win32 Renderer:: Win32 Renderer object
 *
 */
Win32Renderer::Win32Renderer(std::string name, int width, int height) : m_width(width), m_heigt(height)
{
    m_pRenderer = NULL;
    m_wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("MBIMGUI"), NULL};
    ::RegisterClassEx(&m_wc);
    m_hwnd = ::CreateWindow(m_wc.lpszClassName, _T(name.c_str()), MBIMGUI_WINDOW_STYLE, 100, 100, width, height, NULL, NULL, m_wc.hInstance, this);

    m_pRenderer = new Dx12Renderer(m_hwnd);
    if (m_pRenderer == NULL)
    {
        ::UnregisterClass(m_wc.lpszClassName, m_wc.hInstance);
    }
    else
    {
        ::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(m_hwnd);
    }
#if 0
    LONG lStyle = GetWindowLong(m_hwnd, GWL_STYLE);
    lStyle |= WS_THICKFRAME;
    lStyle = lStyle & ~WS_CAPTION;
    SetWindowLong(m_hwnd, GWL_STYLE, lStyle);
#endif
}

HWND Win32Renderer::getWindowHandle()
{
    return m_hwnd;
}

/**
 * @brief Destroy the Win32 Renderer:: Win32 Renderer object
 *
 */
Win32Renderer::~Win32Renderer()
{
    delete m_pRenderer;
}

void Win32Renderer::Render()
{
    m_pRenderer->Render();
    return;
}

bool Win32Renderer::Init()
{
    m_pRenderer->Init();

    ImGuiIO &io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    INT64 perf_frequency, perf_counter;
    if (!::QueryPerformanceFrequency((LARGE_INTEGER *)&perf_frequency))
        return false;
    if (!::QueryPerformanceCounter((LARGE_INTEGER *)&perf_counter))
        return false;

    // Setup backend capabilities flags
    Win32_Data *bd = IM_NEW(Win32_Data)();
    io.BackendPlatformUserData = (void *)bd;
    io.BackendPlatformName = "imgui_impl_win32";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)

    bd->hWnd = (HWND)m_hwnd;
    bd->WantUpdateHasGamepad = true;
    bd->TicksPerSecond = perf_frequency;
    bd->Time = perf_counter;
    bd->LastMouseCursor = ImGuiMouseCursor_COUNT;

    // Set platform dependent data in viewport
    ImGui::GetMainViewport()->PlatformHandleRaw = (void *)m_hwnd;

    // Dynamically load XInput library
#ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
    const char *xinput_dll_names[] =
        {
            "xinput1_4.dll",   // Windows 8+
            "xinput1_3.dll",   // DirectX SDK
            "xinput9_1_0.dll", // Windows Vista, Windows 7
            "xinput1_2.dll",   // DirectX SDK
            "xinput1_1.dll"    // DirectX SDK
        };
    for (int n = 0; n < IM_ARRAYSIZE(xinput_dll_names); n++)
        if (HMODULE dll = ::LoadLibraryA(xinput_dll_names[n]))
        {
            bd->XInputDLL = dll;
            bd->XInputGetCapabilities = (PFN_XInputGetCapabilities)::GetProcAddress(dll, "XInputGetCapabilities");
            bd->XInputGetState = (PFN_XInputGetState)::GetProcAddress(dll, "XInputGetState");
            break;
        }
#endif // IMGUI_IMPL_WIN32_DISABLE_GAMEPAD

    return true;
}

void Win32Renderer::Resize(void *param)
{
    if(m_pRenderer)
    {
        m_pRenderer->Resize(param);
    }
}

void Win32Renderer::Shutdown()
{
    m_pRenderer->Shutdown();
    ImGui_ImplWin32_Shutdown();
}

void Win32Renderer::NewFrame()
{
    m_pRenderer->NewFrame();
    ImGui_ImplWin32_NewFrame();
}

void Win32Renderer::Destroy()
{
    m_pRenderer->Destroy();
    DestroyWindow(m_hwnd);
    UnregisterClass(m_wc.lpszClassName, m_wc.hInstance);
}