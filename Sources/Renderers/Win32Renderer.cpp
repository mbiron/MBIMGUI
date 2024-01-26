#include <tchar.h>
#include <windowsx.h>
#include "Imgui.h"
#include "Win32Renderer.h"
#include "Dx12Renderer.h"
#include "imgui_impl_win32.h"

#define MBIMGUI_WINDOW_STYLE (WS_OVERLAPPEDWINDOW)

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
    case WM_DROPFILES:
        TCHAR buffer[MAX_PATH + 1];
        DragQueryFile((HDROP)wParam, 0, buffer, MAX_PATH + 1);
        pThis->setDragAndDropFileName(std::string(buffer));
        return 0;
        break;
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
Win32Renderer::Win32Renderer(std::string_view name, int width, int height) : m_width(width), m_heigt(height), m_DnDfileName(""), m_dndReceived(false)
{
    m_pRenderer = NULL;
    m_wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("MBIMGUI"), NULL};
    ::RegisterClassEx(&m_wc);
    m_hwnd = ::CreateWindow(m_wc.lpszClassName, _T(name.data()), MBIMGUI_WINDOW_STYLE, 100, 100, width, height, NULL, NULL, m_wc.hInstance, this);

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
}

HWND Win32Renderer::getWindowHandle() const noexcept
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
    bool res = false;

    // DPI_AWARENESS : For now I'm not able to handle DPI awareness properly
    // ImGui_ImplWin32_EnableDpiAwareness();
    res = m_pRenderer->Init();
    if (res)
    {
        res = ImGui_ImplWin32_Init(m_hwnd);
    }
    /* DPI_AWARENESS
        float dpi = ImGui_ImplWin32_GetDpiScaleForHwnd(m_hwnd);
        std::cout << dpi << std::endl;
        POINT temp = {0, 0};
        HMONITOR primaryHandle = MonitorFromPoint(temp, MONITOR_DEFAULTTOPRIMARY);
        dpi = ImGui_ImplWin32_GetDpiScaleForMonitor(primaryHandle);
        std::cout << dpi << std::endl;
    DPI_AWARENESS */
    return res;
}

void Win32Renderer::EnableDragAndDrop()
{
    /* Make file draggable into app */
    DragAcceptFiles(m_hwnd, TRUE);
}
void Win32Renderer::DisableDragAndDrop()
{
    /* Make file draggable into app */
    DragAcceptFiles(m_hwnd, FALSE);
}

void Win32Renderer::setDragAndDropFileName(const std::string &filename) noexcept
{
    m_DnDfileName = filename;
    m_dndReceived = true;
}

bool Win32Renderer::isFileDropped() const noexcept
{
    return m_dndReceived;
}

void Win32Renderer::getDragAndDropFileName(std::string &filename) noexcept
{
    if (m_dndReceived)
    {
        filename = m_DnDfileName;
        m_dndReceived = false;
    }
}

void Win32Renderer::setAPPIcon(int resIcon)
{
    HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(resIcon), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
    SendMessage(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}

void Win32Renderer::Resize(void *param)
{
    if (m_pRenderer)
    {
        m_pRenderer->Resize(param);
    }
}

void Win32Renderer::Shutdown()
{
    m_pRenderer->Shutdown();
    ImGui_ImplWin32_Shutdown();
}

/**
 * @brief Make a new frame for this platform
 *
 */
void Win32Renderer::NewFrame()
{
    m_pRenderer->NewFrame();
    ImGui_ImplWin32_NewFrame();
}

void Win32Renderer::Destroy()
{
    DisableDragAndDrop();
    m_pRenderer->Destroy();
    DestroyWindow(m_hwnd);
    UnregisterClass(m_wc.lpszClassName, m_wc.hInstance);
}