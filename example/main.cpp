#include <iostream>
#include "MBIMGUI.h"

class TestWindow : public MBIWindow
{
private:
public:
    TestWindow(std::string name) : MBIWindow(name,
                                             0,
                                             0,
                                             MBIWindowConfig_hideableInMenu) // Create a entry in the view menu
                                   {};
    void Display()
    {
        ImGui::Text("Hello World !");
    };
};

int main(int argc, char **argv)
{
    /* GUI Configuration */
    MBIConfigFlags flags;

    /* Use a log window. Will be displayed below main central window by default */
    flags = MBIConfig_displayLogWindow;
    /* Use a log bar. */
    // flags = MBIConfig_displayLogBar;

    /* Uncomment for demos */
    // flags |= MBIConfig_displayImGuiDemo;
    // flags |= MBIConfig_displayImPlotDemo;

    /* Uncomment for metrics */
    // flags |= MBIConfig_displayMetrics;

    /* Add menu bar with default options */
    flags |= MBIConfig_displayMenuBar;

    /* Configure logger */
    MBILogger &logger = MBIMGUI::GetLogger();
    logger.Configure(true, "./logfile.txt"); // Active popup on error and set a default logfile

    /* Create GUI */
    MBIMNG gui = MBIMNG("MBIMGUI Test", 1200, 800, flags);

    /* Init GUI */
    if (!gui.Init(14.0))
    {
        std::cerr << "Err init" << std::endl;
        return -1;
    }

    /* Create and setup my windows */
    TestWindow testWindow = TestWindow("Test");

    gui.AddWindow(&testWindow, MBIMGUI::DOCK_MAIN);

    logger.Log(MBILogger::LOG_LEVEL_WARNING, "Hello world");

    /* Start UI */
    gui.Show();

    return 0;
}