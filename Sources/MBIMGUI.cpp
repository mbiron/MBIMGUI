//#include <string>
#include "Imgui.h"
// Internal for advanced docking functions
#include "Imgui_internal.h"
#include "implot.h"
#include "MBIMGUI.h"
#include "MBILogWindow.h"

#include "Win32Renderer.h"

// Check :
// https://github.com/ocornut/imgui/issues/5192
// https://github.com/ocornut/imgui/issues/3350
// https://github.com/ocornut/imgui/issues/4443
//

/***
 *
 * CTOR & DTOR
 *
 */

MBIMGUI::MBIMGUI(const std::string name, int width, int height, MBIConfigFlags flags) : m_name(name), m_logger(MBILogger()), m_confFlags(flags)
{
    m_pRenderer = new Win32Renderer(name, width, height);
    // Default config flags
    m_windowFlags = ImGuiWindowFlags_NoCollapse;

    if (m_confFlags & MBIConfig_displayLogWindow)
    {
        m_windows[DOCK_DOWN] = new MBILogWindow("Logs", m_logger);
    }
    // ImGuiWindowFlags_NoSavedSettings --> Not compatible with default docking
    //| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
}

MBIMGUI::~MBIMGUI()
{
    delete m_pRenderer;
    if (m_confFlags & MBIConfig_displayLogWindow)
    {
        delete m_windows[DOCK_DOWN];
    }
}

/***
 *
 * PRIVATE Functions
 *
 */

void MBIMGUI::SetupDockspace() const
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGuiID dockspaceId = ImGui::GetID("MainDockNode");

    ImGui::DockBuilderRemoveNode(dockspaceId);
    ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);

    // Make the dock node's size and position to match the viewport
    ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);
    ImGui::DockBuilderSetNodePos(dockspaceId, viewport->Pos);

    ImGuiID dock_up_id = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Up, 0.05f, nullptr, &dockspaceId);
    ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Right, 0.25f, nullptr, &dockspaceId);
    ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.2f, nullptr, &dockspaceId);
    ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Down, 0.2f, nullptr, &dockspaceId);
    // ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

    // TODO : Think about ordering (right before down ?)
    for (const auto &member : m_windows)
    {
        ImGuiID dockId = 0;
        switch (member.first)
        {
        case DOCK_DOWN:
            dockId = dock_down_id;
            break;
        case DOCK_UP:
            dockId = dock_up_id;
            break;
        case DOCK_LEFT:
            dockId = dock_left_id;
            break;
        case DOCK_RIGHT:
            dockId = dock_right_id;
            break;
        case DOCK_MAIN:
        default:
            dockId = dockspaceId;
            break;
        }
        ImGui::DockBuilderDockWindow(member.second->GetName().c_str(), dockId);
    }

    ImGui::DockBuilderFinish(dockspaceId);
}

/***
 *
 * PUBLIC Functions
 *
 */

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
    // io.ConfigViewportsNoDecoration = false;
    //  io.ConfigViewportsNoTaskBarIcon = true;

    io.ConfigWindowsMoveFromTitleBarOnly = true;

    // Disable ini file
    io.IniFilename = NULL;
    io.LogFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();
    style.FrameRounding = 8.0;

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
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

void MBIMGUI::AddWindow(MBIWindow *window, MBIDockOption option)
{
    // m_Windows.push_back(window);
    m_windows[option] = window;
}

MBILogger &MBIMGUI::GetLogger()
{
    return m_logger;
}

void MBIMGUI::Show() const
{
    // Main loop
    bool bFirst = true;
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
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

        ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Ensures ImGui fits the window
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiIO &io = ImGui::GetIO();
        if ((io.ConfigFlags & ImGuiConfigFlags_DockingEnable))
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

            // Add this if I want to draw a menu bar above the dockspace (need then to call BeginMenu etc. manually)
            // window_flags |= ImGuiWindowFlags_MenuBar;
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Dockspace", nullptr, window_flags);
            ImGui::PopStyleVar(3);

            ImGuiID dockspaceId = ImGui::GetID("MainDockNode");
            // Submit the DockSpace
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

            if (bFirst)
            {
                SetupDockspace();
                bFirst = false;
            }
            ImGui::End();
        }

        // Call windows
        for (const auto &member : m_windows)
        {
            // ImGui::SetNextWindowSize(win->GetWindowSize(), ImGuiCond_Once);
            ImGui::Begin(member.second->GetName().c_str() /*, nullptr, m_windowFlags*/);
            member.second->Display();
            ImGui::End();
        }

        if (m_confFlags & MBIConfig_displayMetrics)
        {
            ImGui::ShowMetricsWindow();
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
