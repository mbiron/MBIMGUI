#pragma once
#include <string>
#include <map>

// Include everything needed by users
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif // IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "implot.h"

#include "MBIOption.h"
#include "MBIWindow.h"
#include "MBILogger.h"
#include "MBIPlotChart.h"
#include "MBIFileDialog.h"

#include "IconsFontAwesome6.h"

class Renderer;

#define MBIMGUI_VERSION "1.0"

typedef int MBIConfigFlags; ///< See MBIMGUI::_MBIConfigFlags

constexpr char *DEFAULT_OPT_FILE = "./etc/pref.conf"; ///< Default option file

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
        STYLE_VISUAL_DARK,   ///< Custom dark style (Style from [MomoDeve](https://github.com/ocornut/imgui/issues/707#issuecomment-670976818) , slightly modified)
        STYLE_CORPORATE_GREY ///< Custom corporate grey  (Style from [malamanteau](https://github.com/ocornut/imgui/issues/707#issuecomment-468798935))
    } MBIColorStyle;

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
     * @brief Main class of the MBIMGUI framework. You must create an instance of this class and call MBIMNG::Init, MBIMNG::AddWindow and
     * MBIMNG::Show functions to display your UI.
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
            DOCK_DOWN,  ///< The window is docked below the main one
            DOCK_LOG
        } MBIDockOption;

        /**
         * @brief Callback on file open menu
         *
         */
        using MBIOpenFileHandler = void (*)(const std::string &filename);

    private:
        /**
         * Internal types
         */
        using WindowMapPair = std::pair<MBIDockOption, MBIWindow *>; ///< Internal pair for a window and its docking location

        /**
         * Members
         */

        Renderer *m_pRenderer;                               ///< Pointer on the current Renderer of the app.
        std::string m_name;                                  ///< Name of the app (name of the main window)
        std::multimap<MBIDockOption, MBIWindow *> m_windows; ///< Stores all the windows of the app and their location.
        MBIWindow *m_aboutWindow;                            ///< About window of the app

        std::vector<MBIWindow *> m_optionsTabs; ///< Option windows of the app. Will be shown as tab inside the option window in File-->Options
        MBIFileDialog m_logFileDialog;          ///< File browser displayed when setting logfile in option menu
        MBIFileDialog m_openFileDialog;         ///< File browser displayed when oppening file in File menu
        MBIOpenFileHandler m_openFileHandler;
        bool m_dndActiv;

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
        explicit MBIMNG(std::string_view name, int width, int height, const MBIConfigFlags flags = 0);
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
        bool Init(float fontsize = 13.0f, MBIColorStyle eStyle = STYLE_VISUAL_DARK);
        /**
         * @brief Add a window in the application. This window can be docked in the main frame, or free. This may be changed
         * by the user during application lifetime.
         *
         * @param window A pointer to a MBIWindow object.
         * @param option Set the docking state of your window. Choose @ref DOCK_NONE if you want the window to be free by default
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
         * @brief Add a File->Open menu to allow users to load file. Each time a file matching the filters is selected, the openFileHandler is called.
         *
         * @param filters Allowed file filter in the form {".bin"}
         * @param openFileHandler Function called when file is selected
         * @param enableDragAndDrop If true, a file matching the filters can also be drag and drop directly in the application UI.
         */
        void EnableOpenMenu(const std::vector<std::string> &filters, MBIOpenFileHandler openFileHandler, bool enableDragAndDrop = false);

        /**
         * @brief Set the small (title bar) and large (task bar) icon for the app.
         * The resourceIconId shall be defined in RC file (Windows Only).
         *
         * @param resourceIconId Icon ID in application resource file
         */
        void SetAppIcon(int resourceIconId);

        /**
         * @brief Show the application. This will displayed all the windows added by MBIMGUI::AddWindow function
         *
         * @warning This function will block until the application is closed by the user !
         * All your interactive operations must be implemented in the Display functions of your windows.
         */
        void Show();
    };

    /**
     * @brief Save the provided option in a persistent area. After this function return, the option is loadable through LoadOption
     *
     * @tparam T Type of the option
     * @param opt Option to be saved
     */
    template <typename T>
    void SaveOption(const MBIMGUI::MBIOption<T> &opt)
    {
        MBIOPTMGR::WriteOption(opt.getKey(), MBIOPTMGR::OptionToStr(opt.getValue()));
    }

    /**
     * @brief Load the requested option from the persistent area.
     *
     * @tparam T Type of the option
     * @param opt Option to load. Use MBIOption::getValue() to retreive the value.
     * @return true If the option is found.
     * @return false If the option doesn't exist. Use SaveOption to create an entry.
     */
    template <typename T>
    bool LoadOption(MBIMGUI::MBIOption<T> &opt)
    {
        std::string_view str;
        try
        {
            str = MBIOPTMGR::ReadOption(opt.getKey());
        }
        catch (std::out_of_range)
        {
            /* No option, return false */
        }
        if (str.empty())
        {
            return false;
        }
        else
        {
            T val;
            MBIOPTMGR::ConvertOptionValue(str, &val);
            opt.setValue(val);
            return true;
        }
    }

    /**
     * @brief Get the Logger of the application. The logger can be configured (see @ref MBILogger)
     *
     * @return MBILogger& Reference to the logger of the application.
     */
    MBILogger &GetLogger() noexcept;

}; /* namespace MBIMGUI */

/**
 * @brief Extension widgets for [ImGui](https://github.com/ocornut/imgui)
 *
 */
namespace ImGui
{
    /**
     * @brief A combo with an input text and a filter mechanism.
     *
     * This widget is based on idbrii's work from https://github.com/ocornut/imgui/issues/1658#issuecomment-1086193100
     * with modifications proposed by ambrosiogabe.
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
     * @brief Simple circular spinner
     *
     * From ImSpinner by Dalerank
     *
     * @param label Widget label
     * @param color Circle color
     */
    void SimpleCircularSpinner(const char *label, const ImColor &color = 0xffffffff);

    /**
     * @brief Double circular spinner (two circles)
     *
     * From ImSpinner by Dalerank
     *
     * @param label Widget label
     * @param color1 Outer circle color
     * @param color2 Inner circle color
     */
    void DoubleCircularSpinner(const char *label, const ImColor &color1 = 0xffffffff, const ImColor &color2 = 0xff0000ff);

    /**
     * @brief  Heart beat spinner
     *
     * From ImSpinner by Dalerank
     *
     * @param label Widget label
     * @param color Widget color
     */
    void HeartSpinner(const char *label, const ImColor &color = 0xff0000ff);

};