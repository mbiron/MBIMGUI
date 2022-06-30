//#include <string>

// Internal for advanced docking functions
#include "Imgui_internal.h"

#include "MBIMGUI.h"
#include "MBILogWindow.h"
#include "Win32Renderer.h"

MBILogger MBIMGUI::m_logger = MBILogger();

/***
 *
 * CTOR & DTOR
 *
 */

MBIMGUI::MBIMGUI(const std::string name, int width, int height, MBIConfigFlags flags) : m_name(name), m_confFlags(flags), m_logFileDialog(ImGuiFileBrowserFlags_EnterNewFilename)
{
    m_pRenderer = new Win32Renderer(name, width, height);

    if ((m_confFlags & MBIConfig_displayLogWindow) && (m_confFlags & MBIConfig_displayLogBar))
    {
        m_logger.Log(m_logger.LOG_LEVEL_WARNING, "Can't use both log window and log bar. Switching to log window");
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

void StyleCorporateGrey()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    /// 0 = FLAT APPEARENCE
    /// 1 = MORE "3D" LOOK
    int is3D = 0;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
    colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
    colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

    style.PopupRounding = 3;

    style.WindowPadding = ImVec2(4, 4);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(6, 2);

    style.ScrollbarSize = 18;

    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = (float)is3D;

    style.WindowRounding = 3;
    style.ChildRounding = 3;
    style.FrameRounding = 3;
    style.ScrollbarRounding = 2;
    style.GrabRounding = 3;

#ifdef IMGUI_HAS_DOCK
    style.TabBorderSize = (float)is3D;
    style.TabRounding = 3;

    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
#endif
}

void StyleVisualDark()
{
    constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b)
    {
        return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
    };

    auto &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
    const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
    const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

    const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
    const ImVec4 panelHoverColor = ColorFromBytes(29, 151, 236);
    const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);

    const ImVec4 textColor = ColorFromBytes(255, 255, 255);
    const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
    const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

    colors[ImGuiCol_Text] = textColor;
    colors[ImGuiCol_TextDisabled] = textDisabledColor;
    colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
    colors[ImGuiCol_WindowBg] = bgColor;
    colors[ImGuiCol_ChildBg] = bgColor;
    colors[ImGuiCol_PopupBg] = bgColor;
    colors[ImGuiCol_Border] = borderColor;
    colors[ImGuiCol_BorderShadow] = borderColor;
    colors[ImGuiCol_FrameBg] = panelColor;
    colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
    colors[ImGuiCol_FrameBgActive] = panelActiveColor;
    colors[ImGuiCol_TitleBg] = bgColor;
    colors[ImGuiCol_TitleBgActive] = bgColor;
    colors[ImGuiCol_TitleBgCollapsed] = bgColor;
    colors[ImGuiCol_MenuBarBg] = panelColor;
    colors[ImGuiCol_ScrollbarBg] = panelColor;
    colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
    colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
    colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
    colors[ImGuiCol_CheckMark] = panelActiveColor;
    colors[ImGuiCol_SliderGrab] = panelActiveColor;
    colors[ImGuiCol_SliderGrabActive] = panelHoverColor;
    colors[ImGuiCol_Button] = panelColor;
    colors[ImGuiCol_ButtonHovered] = panelHoverColor;
    colors[ImGuiCol_ButtonActive] = panelHoverColor;
    colors[ImGuiCol_Header] = panelColor;
    colors[ImGuiCol_HeaderHovered] = panelHoverColor;
    colors[ImGuiCol_HeaderActive] = panelActiveColor;
    colors[ImGuiCol_Separator] = borderColor;
    colors[ImGuiCol_SeparatorHovered] = borderColor;
    colors[ImGuiCol_SeparatorActive] = borderColor;
    colors[ImGuiCol_ResizeGrip] = bgColor;
    colors[ImGuiCol_ResizeGripHovered] = panelColor;
    colors[ImGuiCol_ResizeGripActive] = lightBgColor;
    colors[ImGuiCol_PlotLines] = panelActiveColor;
    colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
    colors[ImGuiCol_PlotHistogram] = panelActiveColor;
    colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
    /* I do need transparency for my popups */
    // colors[ImGuiCol_ModalWindowDimBg] = bgColor;
    colors[ImGuiCol_DragDropTarget] = bgColor;
    colors[ImGuiCol_NavHighlight] = bgColor;
    colors[ImGuiCol_DockingPreview] = panelActiveColor;
    colors[ImGuiCol_Tab] = bgColor;
    colors[ImGuiCol_TabActive] = panelActiveColor;
    colors[ImGuiCol_TabUnfocused] = bgColor;
    colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
    colors[ImGuiCol_TabHovered] = panelHoverColor;

    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.TabRounding = 0.0f;
}

/***
 *
 * PUBLIC Functions
 *
 */
bool MBIMGUI::Init(float fontsize, const MBIColorStyle eStyle) const
{

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    float scale = 1.0f; // To handle DPI but I'm not able to make it works for now

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

    // Handle fonts
    /*
    io.Fonts->AddFontDefault(); // ProggyClean 13px
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\DroidSans.ttf",fontsize);
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\Cousine-Regular.ttf",fontsize);
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\Karla-Regular.ttf",fontsize);
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\ProggyTiny.ttf",fontsize);
    */
    io.Fonts->AddFontFromFileTTF("..\\..\\..\\Imgui\\imgui\\misc\\fonts\\Roboto-Medium.ttf", fontsize * scale);

    // Setup Dear ImGui style
    switch (eStyle)
    {
    case STYLE_IMGUI_DEFAULT:
        ImGui::StyleColorsDark();
        break;
    case STYLE_IMGUI_DARK:
        ImGui::StyleColorsDark();
        break;
    case STYLE_IMGUI_LIGHT:
        ImGui::StyleColorsLight();
        break;
    case STYLE_VISUAL_DARK:
        StyleVisualDark();
        break;

    case STYLE_CORPORATE_GREY:
    default:
        StyleCorporateGrey();
        break;
    }

    ImGuiStyle &style = ImGui::GetStyle();

    style.FrameRounding = 8.0f;
    style.CellPadding.x = 10.0f;
    style.ItemSpacing.x = 15.0f;
    style.ItemSpacing.y = 6.0f;
    style.FramePadding.x = 6.0f;

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    // DPI stuff : doesn't work ?
    // style.ScaleAllSizes(scale);

    return m_pRenderer->Init();
}

void MBIMGUI::AddAboutWindow(MBIWindow *window)
{
    m_aboutWindow = window;
}

void MBIMGUI::AddWindow(MBIWindow *window, MBIDockOption option)
{
    m_windows[option] = window;
}

void MBIMGUI::AddOptionTab(MBIWindow *window)
{
    m_optionsTabs.push_back(window);
}

MBILogger &MBIMGUI::GetLogger()
{
    return m_logger;
}

void inline MBIMGUI::ShowAboutWindow(bool *openWindow) const
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
        {
            bShowAboutImGui = true;
        }
        if (bShowAboutImGui)
        {
            ImGui::ShowAboutWindow(&bShowAboutImGui);
        }
    }
    ImGui::End();
}

void inline MBIMGUI::ShowOptionWindow(bool &openWindow)
{
    /* Log Config */
    static bool popupOnError = true;
    static bool logToFile = true;

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

        if (ImGui::BeginTabItem("Logs"))
        {
            if (ImGui::Checkbox("Popup on error", &popupOnError))
            {
                m_logger.Configure(popupOnError);
            }
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
            {
                m_logFileDialog.Open();
            }

            m_logFileDialog.Display();
            if (m_logFileDialog.HasSelected())
            {
                std::string fileSelected = m_logFileDialog.GetSelected().string();
                /* Add extension if not specified by user */
                if (fileSelected.compare(fileSelected.size() - 4, 4, ".log") != 0)
                {
                    fileSelected.append(".log");
                }
                m_logger.Log(MBILogger::LOG_LEVEL_INFO, "Selected logfile filename " + fileSelected);
                m_logger.Configure(popupOnError, fileSelected);
                m_logFileDialog.ClearSelected();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    /* Center button relative to window size */
    ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 25, ImGui::GetWindowSize().y - 40));
    if (ImGui::Button("OK##optionWindow", ImVec2(50, 0)))
    {
        openWindow = false;
    }
}

void MBIMGUI::Show()
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
                    {
                        m_logger.Log(MBILogger::LOG_LEVEL_DEBUG, "Not implemented yet :)");
                    }
                    if (ImGui::MenuItem("Log"))
                    {
                        m_logger.Log(MBILogger::LOG_LEVEL_DEBUG, "Welcome in file menu");
                    }

                    ImGui::Separator();
                    ImGui::MenuItem("Options", NULL, &bShowOptions);
                    ImGui::Separator();
                    if (ImGui::MenuItem("Quit"))
                    {
                        bQuit = true;
                    }
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
                {
                    ImPlot::ShowUserGuide();
                }
                ImGui::End();
            }
            if (bShowMetrics)
            {
                ImGui::ShowMetricsWindow(&bShowMetrics);
            }
            if (bShowImguiStyle)
            {
                if (ImGui::Begin("GUI Style editor", &bShowImguiStyle))
                {
                    ImGui::ShowStyleEditor();
                }
                ImGui::End();
            }
            if (bShowImplotStyle)
            {
                if (ImGui::Begin("Plot Style editor", &bShowImplotStyle))
                {
                    ImPlot::ShowStyleEditor();
                }
                ImGui::End();
            }
            if (bShowOptions)
            {
                ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Appearing);
                if (ImGui::Begin("Options", &bShowOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
                {
                    ShowOptionWindow(bShowOptions);
                }
                ImGui::End();
            }
        }

        ImGui::End();

        // Call windows
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
        {
            ImGui::ShowMetricsWindow();
        }
        if (m_confFlags & MBIConfig_displayImGuiDemo)
        {
            ImGui::ShowDemoWindow();
        }
        if (m_confFlags & MBIConfig_displayImPlotDemo)
        {
            ImPlot::ShowDemoWindow();
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
