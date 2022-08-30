#pragma once
#include <Windows.h>
#include <string>
#include "Renderer.h"

/**
 * @brief This class implements the Windows platform backend for ImGui. It contains all the high level functions
 * to initialize draw windows and call the official imgui renderer.
 *
 * It must be based on a engine renderer like DirectX one.
 *
 */
class Win32Renderer : public Renderer
{
private:
    Renderer *m_pRenderer;
    HWND m_hwnd;
    WNDCLASSEX m_wc;
    POINTS m_Pos;
    int m_heigt;
    int m_width;

public:
    /**
     * @brief Construct a new Win 3 2 Renderer object
     *
     * @param name Name of the window, will be displayed in the upper bar.
     * @param width Width of the window of pixels
     * @param height Height of the window of pixels
     */
    Win32Renderer(std::string name, int width, int height);
    ~Win32Renderer();
    bool Init();
    void NewFrame();
    void Shutdown();
    void Render();
    void Resize(void *param);
    void Destroy();
    /**
     * @brief Get the Window Handle object
     *
     * @return HWND Handle of the WINAPI windows handle
     */
    HWND getWindowHandle();
    /**
     * @brief Internal handler to manage windows events
     *
     * @param hwnd
     * @param msg
     * @param wParam
     * @param lParam
     * @return LRESULT
     */
    LRESULT Win32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};