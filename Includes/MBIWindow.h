#pragma once
// Include everything needed for clients
#include "imgui.h"
#include "imfilebrowser.h"
#include "implot.h"
#include "imgui_toggle.h"

#include "MBILogger.h"

namespace MBIMGUI
{

    /**
     * @brief This class described a window in the app. To add a window in your app, you must derivate
     * your classes from this one, implementing the Display method with %ImGui:: calls to create your UI.
     *
     * Each of the MBIWindow classes have an acces to the m_logger member which allows you to configure
     * and use the internal log mechanism.
     *
     */
    class MBIWindow
    {
    public:
        /**
         * @brief Configuration flags for the window, must be passed when constructing the MBIWindow object
         *
         */
        enum _MBIWindowConfigFlags
        {
            MBIWindowConfig_hideableInMenu = 1 << 1, ///< Display the window in the view window, making it hideable/showable from there
        };

        typedef int MBIWindowConfigFlags; ///< Configuration flags for the window, must be passed when constructing the MBIWindow object

    private:
        ImVec2 m_size;                ///< Current size of the window
        ImVec2 m_pos;                 ///< Current position of the window
        std::string m_name;           ///< Name of the window
        bool m_bVisible;              ///< Current visible state of the window
        MBIWindowConfigFlags m_flags; ///< Configuration flags for the window.

    protected:
        /**
         * @brief Logger to display logs to the users (if MBIMGUI::MBIConfig_displayLogWindow is set).
         * See MBILogger for more infos.
         *
         */
        MBILogger &m_logger;
        /**
         * @brief Use this to override default ImGui window behaviour. These flags will be added to default ones configured by MBIMGUI
         *
         */
        ImGuiWindowFlags m_imguiFlags;

    public:
        /**
         * @brief Construct a new MBIWindow object
         *
         * @param name Windows name, will be displayed at the top of the window
         * @param height Windows height, useful only if the window is not docked by default
         * @param width Windows wifth, useful only if the window is not docked by default
         * @param flags Configuration flags, see MBIWindow::_MBIWindowConfigFlags
         */
        MBIWindow(std::string_view name, int height = 0, int width = 0, MBIWindowConfigFlags flags = 0);
        /**
         * @brief Get the Window Size object
         *
         * @return ImVec2
         */
        virtual ImVec2 GetWindowSize() const;

        /**
         * @brief Get the Window Pos object
         *
         * @return ImVec2
         */
        virtual ImVec2 GetWindowPos() const;
        /**
         * @brief Get the the window name
         *
         * @return std::string
         */
        const std::string &GetName() const;

        /**
         * @brief Get the ImGui Flags
         *
         * @return ImGuiWindowFlags
         */
        ImGuiWindowFlags GetFlags() const;

        /**
         * @brief Internal method, called at each frame and call Display method
         *
         */
        void UpdateAndDisplay();

        /**
         * @brief Main method, called at each frame to display your window
         *
         */
        virtual void Display() = 0;
        /**
         * @brief Get the visible state of the window
         *
         * @return true : the window is visible
         * @return false : the window is hidden
         */
        bool IsVisible() const;
        /**
         * @brief Set the visible state of the window
         *
         * @param bVisible : true : the window is visible
         *                   false : the window is hidden
         */
        void SetVisible(bool bVisible);
        /**
         * @brief Test if the window can be hidden/shown from the view menu
         *
         * @return true : the window is in the menu
         * @return false : the window is not in the menu
         */
        bool IsInMenu() const;
    };

}