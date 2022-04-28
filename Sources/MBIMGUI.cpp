#include "Imgui.h"
// Internal for advanced docking functions
#include "Imgui_internal.h"
#include "implot.h"
#include "MBIMGUI.h"
#include "Win32Renderer.h"

// Check :
// https://github.com/ocornut/imgui/issues/5192
// https://github.com/ocornut/imgui/issues/3350
// https://github.com/ocornut/imgui/issues/4443
//

MBIMGUI::MBIMGUI(const std::string name, int width, int height) : m_name(name)
{
    m_pRenderer = new Win32Renderer(name, width, height);
    // Default config flags
    m_windowFlags = ImGuiWindowFlags_NoCollapse;
    // ImGuiWindowFlags_NoSavedSettings --> Not compatible with default docking
    //| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
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

void MBIMGUI::AddWindow(MBIWindow *window, MBIDockOption option)
{
    // m_Windows.push_back(window);
    m_Windows[option] = window;
}

void MBIMGUI::SetupDockspace()
{
    ImGui::DockBuilderRemoveNode(m_dockspaceId);
    ImGui::DockBuilderAddNode(m_dockspaceId, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);

    // Make the dock node's size and position to match the viewport
    ImGui::DockBuilderSetNodeSize(m_dockspaceId, m_viewport->Size);
    ImGui::DockBuilderSetNodePos(m_dockspaceId, m_viewport->Pos);

    ImGuiID dock_up_id = ImGui::DockBuilderSplitNode(m_dockspaceId, ImGuiDir_Up, 0.05f, nullptr, &m_dockspaceId);
    ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(m_dockspaceId, ImGuiDir_Right, 0.25f, nullptr, &m_dockspaceId);
    ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(m_dockspaceId, ImGuiDir_Left, 0.2f, nullptr, &m_dockspaceId);
    ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(m_dockspaceId, ImGuiDir_Down, 0.2f, nullptr, &m_dockspaceId);
    // ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

    // TODO : Think about ordering (right before down ?)
    for (const auto& member : m_Windows)
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
            dockId = m_dockspaceId;
            break;
        }
        ImGui::DockBuilderDockWindow(member.second->GetName().c_str(), dockId);
    }

    ImGui::DockBuilderFinish(m_dockspaceId);
}

void MBIMGUI::Show()
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

        // TODO May be don't set them as members of the class
        m_viewport = ImGui::GetMainViewport();
        m_dockspaceId = ImGui::GetID("MainDockNode");

        // Ensures ImGui fits the window
        ImGui::SetNextWindowPos(m_viewport->Pos);
        ImGui::SetNextWindowSize(m_viewport->Size);
        ImGui::SetNextWindowViewport(m_viewport->ID);

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Dockspace demo - this title will be invisible", nullptr, window_flags);
            ImGui::PopStyleVar(3);

            // Submit the DockSpace
            ImGui::DockSpace(m_dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

            if (bFirst)
            {
                SetupDockspace();
                bFirst = false;
            }
        }

        ImGui::End();

        // Call windows
        for (const auto& member : m_Windows)
        {
            // ImGui::SetNextWindowSize(win->GetWindowSize(), ImGuiCond_Once);
            ImGui::Begin(member.second->GetName().c_str() /*, nullptr, m_windowFlags*/);
            member.second->Display();
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
