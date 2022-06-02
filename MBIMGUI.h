#pragma once
#include <string>
#include <vector>
#include <map>

// Include everything needed for clients
#include "imgui.h"
#include "imfilebrowser.h"
#include "implot.h"

#include "MBIWindow.h"
#include "MBILogger.h"

class Renderer;

/**
 * @brief Main class of the MBIMGUI framework. You must create an instance of this class and call ::Init, ::AddWindow and
 * ::Show functions to display your UI
 *
 */
class MBIMGUI
{
public:
    /**
     * @brief Position of your window in the default docking space (at the program startup). The window may next be moved by the user
     *
     */
    typedef enum _MBIDockOption
    {
        DOCK_NONE,
        DOCK_MAIN,
        DOCK_LEFT,
        DOCK_RIGHT,
        DOCK_UP,
        DOCK_DOWN
    } MBIDockOption;

    enum _MBIConfigFlags
    {
        MBIConfig_displayLogWindow = 1 << 1,  ///< Display a log window, below main central window by default
        MBIConfig_displayMetrics = 1 << 2,    ///< Display the ImGui metrics window by default (useful for debug)
        MBIConfig_displayImGuiDemo = 1 << 3,  ///< Display the ImGui debug window by default (useful for debug)
        MBIConfig_displayImPlotDemo = 1 << 4, ///< Display the ImPlot debug window by default (useful for debug)
        MBIConfig_displayMenuBar = 1 << 5     ///< Display a top menu with default options (closing, hidding/showing window, help...)
    };
    /**
     * @brief Framework configuration flags
     *
     */
    typedef int MBIConfigFlags;

private:
    Renderer *m_pRenderer;
    std::string m_name;
    ImGuiWindowFlags m_windowFlags;
    std::map<MBIDockOption, MBIWindow *> m_windows; // TODO multiple maps ?

    MBIConfigFlags m_confFlags;
    static MBILogger m_logger;

    void SetupDockspace() const;

public:
    /**
     * @brief Construct a new MBIMGUI object. This is the main UI object.
     *
     * @param name Name of your app, displayed in the title bar.
     * @param width Width of the main frame (each window can be docked in this main frame)
     * @param height Height of the main frame (each window can be docked in this main frame)
     * @param flags Framework config flags, see MBIMGUI::_MBIConfigFlags
     */
    MBIMGUI(const std::string name, int width, int height, const MBIConfigFlags flags = 0);
    /**
     * @brief Destroy the MBIMGUI object
     *
     */
    ~MBIMGUI();
    /**
     * @brief Framework initialisation function. This function must be called before any other operation.
     *
     * @param fontsize Set the displayed font size of the app
     * @return true If initialisation is successfull
     * @return false If an error occured
     */
    bool Init(float fontsize = 13.0f) const;
    /**
     * @brief Add a window in the application. This window can be docked in the main frame, or free. This may be changed
     * by the user during application lifetime.
     *
     * @param window A pointer to a MBIWindow object.
     * @param option Set the docking state of your window. Choose MBIDockOption::DOCK_NONE if you want the window to be free by default
     */
    void AddWindow(MBIWindow *window, MBIDockOption option = DOCK_NONE);
    /**
     * @brief Show the application. This will displayed all the windows added by MBIMGUI::AddWindow function
     *
     * @warning This function will block until the application is closed by the user !
     * All your interactive operations must be implemented in the Display functions of your windows.
     */
    void Show();
    /**
     * @brief Get the Logger of the application. The logger can be configurated (see MBILogger class)
     *
     * @return MBILogger& Reference to the logger of the application.
     */
    static MBILogger &GetLogger();

    // TODO : Useful ?
    void SetWindowFlags(const ImGuiWindowFlags flags);
};