//#include <string>

// Internal for advanced docking functions
#include "Imgui_internal.h"

#include "MBIMGUI.h"
#include "MBIMGUI_style.h"
#include "MBILogWindow.h"
#include "Win32Renderer.h"

/***
 *
 * CTOR & DTOR
 *
 */
MBIMGUI::MBIMNG::MBIMNG(const std::string name, int width, int height, MBIConfigFlags flags) : m_name(name),
                                                                                               m_confFlags(flags),
                                                                                               m_logFileDialog(ImGuiFileBrowserFlags_EnterNewFilename),
                                                                                               m_logger(MBIMGUI::GetLogger())
{
    m_pRenderer = new Win32Renderer(name, width, height);

    if ((m_confFlags & MBIConfig_displayLogWindow) && (m_confFlags & MBIConfig_displayLogBar))
    {
        m_logger.Log(LOG_LEVEL_WARNING, "Can't use both log window and log bar. Switching to log window");
        m_confFlags &= ~MBIConfig_displayLogBar;
    }

    if (m_confFlags & MBIConfig_displayLogWindow)
    {
        m_windows[DOCK_DOWN] = new MBILogWindow("Logs", MBILogWindow::MODE_WINDOW);
    }
    else if (m_confFlags & MBIConfig_displayLogBar)
    {
        m_windows[DOCK_DOWN] = new MBILogWindow("Logs", MBILogWindow::MODE_BAR);
    }

    m_logFileDialog.SetTitle("Choose log file");
    m_logFileDialog.SetTypeFilters({".log"});
    m_logFileDialog.SetInputName("logfile.log");

    m_aboutWindow = nullptr;

    // ImGuiWindowFlags_NoSavedSettings --> Not compatible with default docking
    //| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
}

MBIMGUI::MBIMNG::~MBIMNG()
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

void MBIMGUI::MBIMNG::SetupDockspace() const
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGuiID dockspaceId = ImGui::GetID("MainDockNode");

    ImGui::DockBuilderRemoveNode(dockspaceId);
    ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);

    // Make the dock node's size and position to match the viewport
    ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);
    // ImGui::DockBuilderSetNodeSize(dockspaceId,viewport->WorkSize);
    ImGui::DockBuilderSetNodePos(dockspaceId, viewport->Pos);
    // ImGui::DockBuilderSetNodePos(dockspaceId,viewport->WorkPos);

    ImGuiID dock_down_id = 0;
    ImGuiID dock_up_id = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Up, 0.05f, nullptr, &dockspaceId);
    if (m_confFlags & MBIConfig_displayLogBar)
    {
        dock_down_id = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Down, 0.04f, nullptr, &dockspaceId);
    }

    ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Right, 0.40f, nullptr, &dockspaceId);
    ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.2f, nullptr, &dockspaceId);

    if (dock_down_id == 0)
    {
        dock_down_id = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Down, 0.15f, nullptr, &dockspaceId);
    }
    // ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

    for (const auto &member : m_windows)
    {
        ImGuiID dockId = 0;
        switch (member.first)
        {
        case DOCK_DOWN:
            // If log bar, don't show tab bar and make it static
            if (m_confFlags & MBIConfig_displayLogBar)
            {
                ImGuiDockNode *Node = ImGui::DockBuilderGetNode(dock_down_id);
                Node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoResizeY | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit;
            }
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
            dockId = dockspaceId;
            break;
        case DOCK_NONE:
        default:
            continue;
        }

        ImGui::DockBuilderDockWindow(member.second->GetName().c_str(), dockId);
    }
    ImGui::DockBuilderFinish(dockspaceId);
}

inline void MBIMGUI::MBIMNG::ShowAboutWindow(bool *openWindow) const
{
    static bool bShowAboutImGui = false;
    if (m_aboutWindow != nullptr)
    {
        if (ImGui::Begin(m_aboutWindow->GetName().c_str(), openWindow, ImGuiWindowFlags_AlwaysAutoResize))
        {
            m_aboutWindow->Display();
            ImGui::Separator();
        }
    }
    else
    {
        ImGui::Begin("About MBIMGUI", openWindow, ImGuiWindowFlags_AlwaysAutoResize);
    }

    if ((*openWindow) == true)
    {
        ImGui::Text("This application has been made using MBIMGUI V" MBIMGUI_VERSION);
        ImGui::Text("MBIMGUI is a simple overlay above ImGui and ImPlot frameworks.");

        if (ImGui::Button("Show ImGui Infos"))
            bShowAboutImGui = true;

        if (bShowAboutImGui)
            ImGui::ShowAboutWindow(&bShowAboutImGui);
    }
    ImGui::End();
}

inline void MBIMGUI::MBIMNG::ShowOptionWindow(bool &openWindow)
{
    /* Log Config */
    static bool popupOnError = true;
    static bool logToFile = true;
    static int choosenStyle = MBIMGUI::GetStyle();

    if (ImGui::BeginTabBar("MyTabBar"))
    {
        for (auto tab : m_optionsTabs)
        {
            if (ImGui::BeginTabItem(tab->GetName().c_str()))
            {
                tab->Display();
                ImGui::EndTabItem();
            }
        }

        if (ImGui::BeginTabItem("Logs & Style"))
        {
            ImGui::Text("Logs configuration");
            ImGui::Separator();
            if (ImGui::Checkbox("Popup on error", &popupOnError))
                m_logger.Configure(popupOnError);

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("When activated, any occuring error will be displayed in a popup in addition to the log window");

            ImGui::SameLine();
            if (ImGui::Checkbox("Log to file", &logToFile))
            {
                if (!logToFile)
                {
                    m_logger.Configure(popupOnError);
                }
                else
                {
                    if (m_logger.GetLogFullFileName() == "")
                    {
                        // Force user to choose a valid file
                        m_logFileDialog.Open();
                    }
                }
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(std::string("When activated, all logs are written to : " + m_logger.GetLogFullFileName()).c_str());

            /* Open file dialog when user clicks this button */
            if (ImGui::Button("Choose log file"))
                m_logFileDialog.Open();

            m_logFileDialog.Display();
            if (m_logFileDialog.HasSelected())
            {
                std::string fileSelected = m_logFileDialog.GetSelected().string();
                /* Add extension if not specified by user */
                if (fileSelected.compare(fileSelected.size() - 4, 4, ".log") != 0)
                {
                    fileSelected.append(".log");
                }
                m_logger.Log(LOG_LEVEL_INFO, "Selected logfile filename " + fileSelected);
                m_logger.Configure(popupOnError, fileSelected);
                m_logFileDialog.ClearSelected();
            }

            ImGui::Spacing();
            ImGui::Text("Style configuration");
            ImGui::Separator();
            if (ImGui::Combo("Style", &choosenStyle, "ImGui Default\0ImGui Dark\0ImGui Light\0Visual Dark\0Corporate Grey\0", 5))
                MBIMGUI::SetStyle((MBIColorStyle)choosenStyle);

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    /* Center button relative to window size */
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 25, ImGui::GetWindowSize().y - 40));
    if (ImGui::Button("OK##optionWindow", ImVec2(50, 0)))
        openWindow = false;
}

/***
 *
 * PUBLIC Functions
 *
 */
bool MBIMGUI::MBIMNG::Init(float fontsize, const MBIColorStyle eStyle) const
{
    /* Setup Dear ImGui context */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    float scale = 1.0f; // To handle DPI but I'm not able to make it works for now

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
                                                        // io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts // TO TEST !
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    /* Disable ini file */
    io.IniFilename = NULL;
    io.LogFilename = NULL;

    /* Handle fonts */
    /*
    io.Fonts->AddFontDefault(); // ProggyClean 13px
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\DroidSans.ttf",fontsize);
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\Cousine-Regular.ttf",fontsize);
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\Karla-Regular.ttf",fontsize);
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\ProggyTiny.ttf",fontsize);
    */
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\Roboto-Medium.ttf", fontsize * scale);

    /* Style configuration */
    MBIMGUI::SetStyle(eStyle);

    // DPI stuff : doesn't work ?
    // style.ScaleAllSizes(scale);

    return m_pRenderer->Init();
}

void MBIMGUI::MBIMNG::AddAboutWindow(MBIWindow *window)
{
    m_aboutWindow = window;
}

void MBIMGUI::MBIMNG::AddWindow(MBIWindow *window, MBIDockOption option)
{
    m_windows[option] = window;
}

void MBIMGUI::MBIMNG::AddOptionTab(MBIWindow *window)
{
    m_optionsTabs.push_back(window);
}

void MBIMGUI::MBIMNG::Show()
{
    // Main loop
    bool bFirst = true;
    bool bQuit = false;

    while (!bQuit)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                bQuit = true;
        }
        if (bQuit)
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

        // Handle docking
        if ((io.ConfigFlags & ImGuiConfigFlags_DockingEnable))
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

            // Add this if I want to draw a menu bar above the dockspace (need then to call BeginMenu etc. manually)
            if (m_confFlags & MBIConfig_displayMenuBar)
            {
                window_flags |= ImGuiWindowFlags_MenuBar;
            }
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
        }

        // Create menu
        if (m_confFlags & MBIConfig_displayMenuBar)
        {
            static bool bShowMetrics = false;
            static bool bShowImguiStyle = false;
            static bool bShowImplotStyle = false;
            static bool bShowAbout = false;
            static bool bShowOptions = false;
            static bool bShowGraphHelp = false;
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open"))
                        m_logger.Log(LOG_LEVEL_DEBUG, "Not implemented yet :)");

                    if (ImGui::MenuItem("Log"))
                        m_logger.Log(LOG_LEVEL_DEBUG, "Welcome in file menu");

                    ImGui::Separator();
                    ImGui::MenuItem("Options", NULL, &bShowOptions);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Quit"))
                        bQuit = true;

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("View"))
                {
                    // Set windows in menu file
                    for (const auto &member : m_windows)
                    {
                        if (member.second->IsInMenu())
                        {
                            bool bShowWindow = member.second->IsVisible();
                            if (ImGui::MenuItem(member.second->GetName().c_str(), NULL, bShowWindow))
                            {
                                bShowWindow = !bShowWindow;
                                member.second->SetVisible(bShowWindow);
                            }
                        }
                    }
                    ImGui::Separator();
                    ImGui::MenuItem("HMI Debug/Metrics", NULL, &bShowMetrics);
                    ImGui::MenuItem("GUI Style editor", NULL, &bShowImguiStyle);
                    ImGui::MenuItem("Plot Style editor", NULL, &bShowImplotStyle);

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help"))
                {
                    ImGui::MenuItem("About", NULL, &bShowAbout);
                    ImGui::MenuItem("Graph user guide", NULL, &bShowGraphHelp);
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            if (bShowAbout)
            {
                ShowAboutWindow(&bShowAbout);
            }
            if (bShowGraphHelp)
            {
                if (ImGui::Begin("Graph user guide", &bShowGraphHelp))
                    ImPlot::ShowUserGuide();

                ImGui::End();
            }
            if (bShowMetrics)
                ImGui::ShowMetricsWindow(&bShowMetrics);

            if (bShowImguiStyle)
            {
                if (ImGui::Begin("GUI Style editor", &bShowImguiStyle))
                    ImGui::ShowStyleEditor();

                ImGui::End();
            }
            if (bShowImplotStyle)
            {
                if (ImGui::Begin("Plot Style editor", &bShowImplotStyle))
                    ImPlot::ShowStyleEditor();

                ImGui::End();
            }
            if (bShowOptions)
            {
                ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Appearing);
                if (ImGui::Begin("Options", &bShowOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
                    ShowOptionWindow(bShowOptions);

                ImGui::End();
            }
        }

        ImGui::End();

        /* Call windows */
        for (const auto &member : m_windows)
        {
            if (member.second->IsVisible())
            {
                if (member.first == DOCK_NONE)
                    ImGui::SetNextWindowSize(member.second->GetWindowSize(), ImGuiCond_Once);

                // Do not use ImGuiWindowFlags_AlwaysAutoResize. It has a strange behaviour with Implot Windows (TODO : Open an issue to epezent)
                if (ImGui::Begin(member.second->GetName().c_str(), NULL, ImGuiWindowFlags_NoCollapse | member.second->GetFlags()))
                {
                    member.second->Display();
                }
                ImGui::End();
            }
        }

        if (m_confFlags & MBIConfig_displayMetrics)
            ImGui::ShowMetricsWindow();

        if (m_confFlags & MBIConfig_displayImGuiDemo)
            ImGui::ShowDemoWindow();

        if (m_confFlags & MBIConfig_displayImPlotDemo)
            ImPlot::ShowDemoWindow();

        /* Rendering */
        ImGui::Render();
        m_pRenderer->Render();
    }

    /* Cleanup */
    m_pRenderer->Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    m_pRenderer->Destroy();

    return;
}
