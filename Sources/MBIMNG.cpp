#include <sstream>

// Internal for advanced docking functions
#include "Imgui_internal.h"

#include "MBIMGUI.h"
#include "MBIMGUI_style.h"
#include "MBILogWindow.h"
#include "Win32Renderer.h"

namespace MBIMGUI
{
    static std::map<std::string, std::string> g_optionMap;

    namespace
    {
        static std::string g_optionFile = "./etc/pref.conf";

        void ReadAllOptions()
        {
            std::ifstream ifile;
            std::string szLine;
            std::string key;
            std::string val;

            szLine.reserve(128);

            ifile.open(g_optionFile, std::ifstream::in);
            while (std::getline(ifile, szLine))
            {
                std::istringstream istream(szLine);
                /* Parse line */
                if ((istream >> key >> std::string(" = ") >> val))
                {
                    g_optionMap[key] = val;
                }
            }
        }

        void WriteAllOptions()
        {
            std::ofstream ofile;

            ofile.open(g_optionFile, std::ofstream::out);

            for (auto it : g_optionMap)
            {
                ofile << it.first << " = " << it.second << std::endl;
            }
        }
    }

    void MBIOPTMGR::WriteOption(std::string_view key, std::string_view val)
    {
        g_optionMap[key.data()] = val;
    }

    const std::string &MBIOPTMGR::ReadOption(std::string_view key)
    {
        // TODO : Find a better error management...
        static const std::string emptyStr = "";
        if (g_optionMap.find(key.data()) == g_optionMap.end())
        {
            return emptyStr;
        }
        else
        {
            return g_optionMap.at(key.data());
        }
    }
}

inline const std::wstring s2ws(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

/***
 *
 * CTOR & DTOR
 *
 */
MBIMGUI::MBIMNG::MBIMNG(std::string_view name, int width, int height, MBIConfigFlags flags) : m_name(name),
                                                                                              m_confFlags(flags),
                                                                                              m_logFileDialog(ImGuiFileBrowserFlags_EnterNewFilename),
                                                                                              m_openFileDialog(),
                                                                                              m_openFileHandler(nullptr),
                                                                                              m_dndActiv(false),
                                                                                              m_aboutWindow(nullptr),
                                                                                              m_logger(MBIMGUI::GetLogger())
{
    /* ImGui win32 backend handle Wchar for multiple languages. For now, keep it simple */
    // std::wstring wname = s2ws(name.data());
    m_pRenderer = new Win32Renderer(name, width, height);

    if ((m_confFlags & MBIConfig_displayLogWindow) && (m_confFlags & MBIConfig_displayLogBar))
    {
        m_logger.Log(LOG_LEVEL_WARNING, "Can't use both log window and log bar. Switching to log window");
        m_confFlags &= ~MBIConfig_displayLogBar;
    }

    if (m_confFlags & MBIConfig_displayLogWindow)
    {
        m_windows.insert(WindowMapPair(DOCK_LOG, new MBILogWindow(ICON_FA_BOOK " Logs", MBILogWindow::MODE_WINDOW)));
    }
    else if (m_confFlags & MBIConfig_displayLogBar)
    {
        m_windows.insert(WindowMapPair(DOCK_LOG, new MBILogWindow(ICON_FA_BOOK " Logs", MBILogWindow::MODE_BAR)));
    }

    m_logFileDialog.SetTitle("Choose log file");
    m_logFileDialog.SetTypeFilters({".log"});
    m_logFileDialog.SetInputName("logfile.log");

    m_openFileDialog.SetTitle("Open file");

    /* Option management : retreive all stored options */
    MBIMGUI::ReadAllOptions();
}

MBIMGUI::MBIMNG::~MBIMNG()
{
    /* Save all options before leaving */
    MBIMGUI::WriteAllOptions();

    delete m_pRenderer;
    if (m_confFlags & MBIConfig_displayLogWindow)
    {
        /* Delete log window if exists */
        delete m_windows.find(DOCK_LOG)->second;
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
        case DOCK_LOG:
            // If log bar, don't show tab bar and make it static
            if (m_confFlags & MBIConfig_displayLogBar)
            {
                ImGuiDockNode *Node = ImGui::DockBuilderGetNode(dock_down_id);
                Node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton |
                                    ImGuiDockNodeFlags_NoResizeX | ImGuiDockNodeFlags_NoResizeY | ImGuiDockNodeFlags_NoDockingOverMe |
                                    ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit;
            }
            dockId = dock_down_id;
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
        ImGui::Begin(ICON_FA_INFO_CIRCLE " About MBIMGUI", openWindow, ImGuiWindowFlags_AlwaysAutoResize);
    }

    if ((*openWindow) == true)
    {
        ImGui::Text("This application has been made using MBIMGUI V" MBIMGUI_VERSION);
        ImGui::Text("Build on %s %s", __DATE__, __TIME__);
        ImGui::Text("MBIMGUI is a simple overlay above ImGui and ImPlot frameworks.");

        if (ImGui::Button(ICON_FA_QUESTION_CIRCLE " Show ImGui Infos"))
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

        if (ImGui::BeginTabItem(ICON_FA_PAINT_BRUSH " Logs & Style"))
        {
            //ImGui::Text( ICON_FA_FILE_ARCHIVE " Logs configuration");
            ImGui::SeparatorText(ICON_FA_FILE_ARCHIVE " Logs configuration");
            if (ImGui::Checkbox(ICON_FA_WINDOW_RESTORE " Popup on error", &popupOnError))
                m_logger.Configure(popupOnError);

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("When activated, any occuring error will be displayed in a popup in addition to the log window");

            ImGui::SameLine();
            if (ImGui::Checkbox(ICON_FA_DATABASE " Log to file", &logToFile))
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
            //ImGui::Text(ICON_FA_PALETTE " Style configuration");
            ImGui::SeparatorText(ICON_FA_PALETTE " Style configuration");
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
bool MBIMGUI::MBIMNG::Init(float fontsize, const MBIColorStyle eStyle)
{
    /* Setup Dear ImGui context */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    const float scale = 1.0f; // To handle DPI but I'm not able to make it works for now
    const float correctedSize = fontsize * scale;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
                                                        // io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts // TO TEST !
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    /* Set ini file */
    io.IniFilename = ".\\ImGui.ini";
    /* Disable log file */
    io.LogFilename = NULL;

    /* Handle fonts */
    /* Try loading from installation dir */
    if (io.Fonts->AddFontFromFileTTF(".\\fonts\\Roboto-Medium.ttf", correctedSize) == nullptr)
    {
        /* Try loading from imgui dir */
        io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\Roboto-Medium.ttf", fontsize * scale);
    }

    /* Merge in icons from Font Awesome */
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0}; // Will not be copied by AddFont* so keep in scope using static.
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(".\\fonts\\" FONT_ICON_FILE_NAME_FAS, correctedSize, &icons_config, icons_ranges);
    io.Fonts->Build();

    /* Style configuration */
    MBIMGUI::SetStyle(eStyle);

    /* Make auto fit leave a 5% space to the fit extents of X and Y */
    ImPlot::GetStyle().FitPadding = ImVec2(0.05f, 0.05f);

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
    m_windows.insert(WindowMapPair(option, window));
}

void MBIMGUI::MBIMNG::AddOptionTab(MBIWindow *window)
{
    m_optionsTabs.push_back(window);
}

void MBIMGUI::MBIMNG::EnableOpenMenu(const std::vector<std::string> &filters,
                                     MBIOpenFileHandler openFileHandler,
                                     bool enableDragAndDrop)
{
    m_openFileDialog.SetTypeFilters(filters);
    m_openFileHandler = openFileHandler;
    m_dndActiv = enableDragAndDrop;
    if (m_dndActiv)
    {
        // Specific for now
        ((Win32Renderer *)m_pRenderer)->EnableDragAndDrop();
    }
}

void MBIMGUI::MBIMNG::Show()
{
    // Main loop
    bool bQuit = false;
    bool bResetDockspace = false;

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

            /* If dockspace already exists, it means it has been loaded according to user-specifics layout modifications
            So we don't need to setup dockspace to its default position, unless a reset has been requested */
            if (bResetDockspace ||
                (ImGui::DockBuilderGetNode(dockspaceId) == NULL))
            {
                // Submit the DockSpace before setup
                ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

                SetupDockspace();
                bResetDockspace = false;
            }
            else
            {
                // Submit the DockSpace previously saved
                ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
            }
        }

        // Create menu
        if (m_confFlags & MBIConfig_displayMenuBar)
        {
#ifdef _DEBUG
            static bool bShowMetrics = false;
            static bool bShowImguiStyle = false;
            static bool bShowImplotStyle = false;
#endif
            static bool bShowAbout = false;
            static bool bShowOptions = false;
            static bool bShowGraphHelp = false;
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (m_openFileHandler)
                    {
                        if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open"))
                        {
                            m_openFileDialog.Open();
                        }
                        ImGui::Separator();
                    }
                    ImGui::MenuItem(ICON_FA_WRENCH " Options", NULL, &bShowOptions);
                    ImGui::Separator();

                    if (ImGui::MenuItem(ICON_FA_DOOR_CLOSED " Quit"))
                        bQuit = true;

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("View"))
                {
                    bResetDockspace = ImGui::MenuItem(ICON_FA_ARROW_CIRCLE_LEFT " Reset to default layout");
                    ImGui::Separator();
                    ImGui::SeparatorText("Windows");
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
#ifdef _DEBUG
                    ImGui::SeparatorText("Debug");
                    ImGui::MenuItem("HMI Debug/Metrics", NULL, &bShowMetrics);
                    ImGui::MenuItem("GUI Style editor", NULL, &bShowImguiStyle);
                    ImGui::MenuItem("Plot Style editor", NULL, &bShowImplotStyle);
#endif
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help"))
                {
                    ImGui::MenuItem(ICON_FA_QUESTION_CIRCLE " About", NULL, &bShowAbout);
                    ImGui::MenuItem(ICON_FA_CHART_LINE " Graph user guide", NULL, &bShowGraphHelp);
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
#ifdef _DEBUG
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
#endif
            if (bShowOptions)
            {
                ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Appearing);
                if (ImGui::Begin("Options", &bShowOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
                    ShowOptionWindow(bShowOptions);

                ImGui::End();
            }

            m_openFileDialog.Display();
            if (m_openFileDialog.HasSelected())
            {
                std::string fileSelected = m_openFileDialog.GetSelected().string();
                /* Call user hadler */
                m_openFileHandler(fileSelected);
                m_openFileDialog.ClearSelected();
            }
        }
        ImGui::End();

        /* Handle Drag And Drop */
        if (m_dndActiv && ((Win32Renderer *)m_pRenderer)->isFileDropped())
        {
            /* Get filename */
            std::string filename;
            ((Win32Renderer *)m_pRenderer)->getDragAndDropFileName(filename);
            /* Call user function */
            m_openFileHandler(filename);
        }

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
                    member.second->UpdateAndDisplay();
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