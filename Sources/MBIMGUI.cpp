#include "Imgui.h"
#include "MBIMGUI.h"
#include "Win32Renderer.h"

MBIMGUI::MBIMGUI(MBIMGUI_Callback cb, void *arg, int width, int height) : m_cb(cb), m_cbArg(arg), m_width(width), m_height(height)
{
    m_pRenderer = new Win32Renderer(width, height);
}

MBIMGUI::~MBIMGUI()
{
    delete m_pRenderer;
}

bool MBIMGUI::Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    return m_pRenderer->Init();
}

void MBIMGUI::Display()
{
    bool show_demo_window = true;
    bool show_another_window = false;

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        m_pRenderer->NewFrame();
        ImGui::NewFrame();

// Ensures ImGui fits the window
#if 1
        RECT rect;
        if (GetWindowRect(((Win32Renderer *)m_pRenderer)->getWindowHandle(), &rect))
        {
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            // For i don't now why 
            ImGui::SetNextWindowSize(ImVec2(width, height)); 
        }
        ImGui::SetNextWindowPos(ImVec2(0, 0));
#endif

        // CALL USER FONCTION
        m_cb(m_cbArg);

        // Rendering
        ImGui::Render();
        m_pRenderer->Render();
    }

    // Cleanup
    m_pRenderer->Shutdown();
    ImGui::DestroyContext();

    m_pRenderer->Destroy();

    return;
}
