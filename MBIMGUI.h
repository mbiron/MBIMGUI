#pragma once
#include <string>
#include <map>

// Include everything needed by users
#include "imgui.h"
#include "imfilebrowser.h"
#include "implot.h"
#include "imgui_memory_editor.h"

#include "MBIWindow.h"
#include "MBILogger.h"

class Renderer;

#define MBIMGUI_VERSION "1.0"

typedef int MBIConfigFlags; ///< See MBIMGUI::_MBIConfigFlags

/**
 * @brief Main namespace of the framework. Contains global functions
 *
 */
namespace MBIMGUI
{
    /**
     * @brief Color scheme used for the application
     *
     */
    typedef enum _MBIColorStyle
    {
        STYLE_IMGUI_DEFAULT, ///< Default Imgui color style
        STYLE_IMGUI_DARK,    ///< Default dark Imgui color style
        STYLE_IMGUI_LIGHT,   ///< Default light Imgui color style
        STYLE_VISUAL_DARK,   ///< Custom dark style (Style from @MomoDeve : https://github.com/ocornut/imgui/issues/707#issuecomment-670976818 , slightly modified)
        STYLE_CORPORATE_GREY ///< Custom corporate grey  (Style from @malamanteau : https://github.com/ocornut/imgui/issues/707#issuecomment-468798935)
    }MBIColorStyle;

    /**
     * @brief Framework configuration flags
     *
     */
    enum _MBIConfigFlags
    {
        MBIConfig_displayLogWindow = 1 << 1,  ///< Display a log window, below main central window by default
        MBIConfig_displayMetrics = 1 << 2,    ///< Display the ImGui metrics window by default (useful for debug)
        MBIConfig_displayImGuiDemo = 1 << 3,  ///< Display the ImGui debug window by default (useful for debug)
        MBIConfig_displayImPlotDemo = 1 << 4, ///< Display the ImPlot debug window by default (useful for debug)
        MBIConfig_displayMenuBar = 1 << 5,    ///< Display a top menu with default options (closing, hidding/showing window, help...)
        MBIConfig_displayLogBar = 1 << 6      ///< Display a log bar at the bottom of the main window
    };

    /**
     * @brief Main class of the MBIMGUI framework. You must create an instance of this class and call ::Init, ::AddWindow and
     * ::Show functions to display your UI
     *
     */
    class MBIMNG
    {
    public:
        /**
         * @brief Position of your window in the default docking space (at the program startup). The window may next be moved by the user
         *
         */
        typedef enum _MBIDockOption
        {
            DOCK_NONE,  ///< Do not dock the window. The window is draw inside an independant OS window
            DOCK_MAIN,  ///< The window is the main one. The window is draw inside the main OS window
            DOCK_LEFT,  ///< The window is docked at the left of the main one
            DOCK_RIGHT, ///< The window is docked at the right of the main one
            DOCK_UP,    ///< The window is docked above the main one
            DOCK_DOWN   ///< The window is docked below the main one
        } MBIDockOption;

    private:
        Renderer *m_pRenderer;                          ///< Pointer on the current Renderer of the app.
        std::string m_name;                             ///< Name of the app (name of the main window)
        std::map<MBIDockOption, MBIWindow *> m_windows; ///< Stores all the windows of the app and their location. @TODO : Use multiple map to tab multiple docked windows ?
        MBIWindow *m_aboutWindow;                       ///< About window of the app

        std::vector<MBIWindow *> m_optionsTabs; ///< Option windows of the app. Will be shown as tab inside the option window in File-->Options
        ImGui::FileBrowser m_logFileDialog;     ///< File browser displayed when setting logfile in option menu

        MBIConfigFlags m_confFlags; ///< Current framework flags
        MBILogger &m_logger;        ///< Logger of the application

        /**
         * @brief Setup the ImGui dockspace layout. Split the current viewport in multiple dockspace so the windows may be docked into them.
         *
         */
        void SetupDockspace() const;
        /**
         * @brief Display the option window (File-->Options)
         *
         * @param closeWindow As for ImGui::Begin, passing closeWindow displays a Close button on the upper-right corner of the window, the pointed value will be set to false when the button is pressed.
         */
        void ShowOptionWindow(bool &closeWindow);
        /**
         * @brief Display the about window (Help-->About)
         *
         * @param openWindow As for ImGui::Begin, passing closeWindow displays a Close button on the upper-right corner of the window, the pointed value will be set to false when the button is pressed.
         */
        void ShowAboutWindow(bool *openWindow) const;

    public:
        /**
         * @brief Construct a new MBIMGUI object. This is the main UI object.
         *
         * @param name Name of your app, displayed in the title bar.
         * @param width Width of the main frame (each window can be docked in this main frame)
         * @param height Height of the main frame (each window can be docked in this main frame)
         * @param flags Framework config flags, see MBIMGUI::_MBIConfigFlags
         */
        MBIMNG(const std::string name, int width, int height, const MBIConfigFlags flags = 0);
        /**
         * @brief Destroy the MBIMGUI object
         *
         */
        ~MBIMNG();
        /**
         * @brief Framework initialisation function. This function must be called before any other operation.
         *
         * @param fontsize Set the displayed font size of the app
         * @param eStyle Color style for the app
         * @return true If initialisation is successfull
         * @return false If an error occured
         */
        bool Init(float fontsize = 13.0f, MBIColorStyle eStyle = STYLE_VISUAL_DARK) const;
        /**
         * @brief Add a window in the application. This window can be docked in the main frame, or free. This may be changed
         * by the user during application lifetime.
         *
         * @param window A pointer to a MBIWindow object.
         * @param option Set the docking state of your window. Choose MBIDockOption::DOCK_NONE if you want the window to be free by default
         */
        void AddWindow(MBIWindow *window, MBIDockOption option = DOCK_NONE);

        /**
         * @brief Add a window to be displayed in the about menu.
         *
         * @param window A pointer to a MBIWindow object.
         */
        void AddAboutWindow(MBIWindow *window);

        /**
         * @brief Add a tab to be displayed in the option menu.
         *
         * @param window A pointer to a MBIWindow object.
         */
        void AddOptionTab(MBIWindow *window);

        /**
         * @brief Show the application. This will displayed all the windows added by MBIMGUI::AddWindow function
         *
         * @warning This function will block until the application is closed by the user !
         * All your interactive operations must be implemented in the Display functions of your windows.
         */
        void Show();
    };

    /**
     * @brief Get the Logger of the application. The logger can be configured (see @ref MBILogger)
     *
     * @return MBILogger& Reference to the logger of the application.
     */
    MBILogger &GetLogger();

    // TODO : Persistent option mechanism ?
    /* Use std::pair ?

    void SaveOption(const MBIOption& opt);
    void LoadOption(MBIOption& opt);
    const std::vector<MBIOption>& LoadAllOption();

    template<typename T>
    class MBIOption
    {
        private:
            std::pair<std::string, T> opt;
        public:
            MBIOption() = delete;
            MBIOption(std::string name);
            MBIOption(std::string name, T value);

            std::string toString() const; // To serialize to file
            void setValue(T value);
            T getValue() const;

    }
    */
};

/**
 * @brief Extension widgets for ImGui
 *
 */
namespace ImGui
{
    /**
     * @brief A combo with an input text and a filter mechanism.
     *
     * @param label Widget label
     * @param current_item Current item selected in the list
     * @param items List of items
     * @param popup_max_height_in_items Maximum number of items to be displayed in the popup. Set -1 for default.
     * @return true if value has been modified in the previous frame
     * @return false otherwise
     */
    bool ComboWithFilter(const char *label, int *current_item, const std::vector<std::string> &items, int popup_max_height_in_items = -1);

    /**
     * @brief A basic toggle button
     *
     * @param str_id Widget label
     * @param v Value retreive by the button
     * @return true if value has been modified in the previous frame
     * @return false otherwise
     */
    bool ToggleButton(const char *str_id, bool *v);

    /**
     * @brief Simple circular spinner
     *
     * @param label Widget label
     * @param color Circle color
     * @param bgColor Background color (must be the same as the current background theme color)
     */
    void SimpleCircularSpinner(const char *label, const ImColor &color = 0xffffffff);

    /**
     * @brief Double circular spinner (two circles)
     *
     * @param label Widget label
     * @param color1 Outer circle color
     * @param color2 Inner circle color
     */
    void DoubleCircularSpinner(const char *label, const ImColor &color1 = 0xffffffff, const ImColor &color2 = 0xff0000ff);

};