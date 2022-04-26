#include "Imgui.h"
#include "implot.h"
#include "MBIMGUI.h"
#include "Win32Renderer.h"

MBIMGUI::MBIMGUI(const std::string name, MBIWindow &window) : m_name(name), m_window(window)
{
    m_pRenderer = new Win32Renderer(name, window.GetWindowSize().x, window.GetWindowSize().y);
    // Default config flags
    m_windowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
}

MBIMGUI::~MBIMGUI()
{
    delete m_pRenderer;
}

bool MBIMGUI::Init() const
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Disable ini file 
    io.IniFilename = NULL;
    io.LogFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    return m_pRenderer->Init();
}

void MBIMGUI::SetWindowFlags(ImGuiWindowFlags flags)
{
    m_windowFlags = flags;
}

void MBIMGUI::AddChildWindow(MBIWindow *window)
{
    m_secWindows.push_back(window);
}

void MBIMGUI::Show() const
{
    bool show_demo_window = true;
    bool show_another_window = false;
    bool bOpened = true;
    // Main loop
    bool done = false;
    while (!done && bOpened)
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
#ifdef IMGUI_HAS_VIEWPORT
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
#else
        RECT rect;
        if (GetWindowRect(((Win32Renderer *)m_pRenderer)->getWindowHandle(), &rect))
        {
            LONG width = rect.right - rect.left;
            LONG height = rect.bottom - rect.top;
            // For i don't now why
            // ImGui::SetNextWindowSize(ImVec2(static_cast<float>(width), static_cast<float>(height)));
            ImGui::SetNextWindowSize(ImVec2(width, height));
        }
        ImGui::SetNextWindowPos(ImVec2(0, 0));
#endif

        // CALL MAIN Window
        ImGui::Begin(m_window.GetName().c_str(), NULL, m_windowFlags);
        m_window.Display();
        ImGui::End();

        // Call children
        for (MBIWindow *win : m_secWindows)
        {
            ImGui::SetNextWindowSize(win->GetWindowSize(), ImGuiCond_Once);
            ImGui::Begin(win->GetName().c_str());
            win->Display();
            ImGui::End();
        }
        // Rendering
        ImGui::Render();
        m_pRenderer->Render();
    }

    // Cleanup
    m_pRenderer->Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    m_pRenderer->Destroy();

    return;
}
