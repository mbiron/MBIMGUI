#include <iostream>
#include "MBIMGUI.h"
#include "TestWindow.h"
#include "DemoWindow.h"


int main(int argc, char **argv)
{
    TestWindow window = TestWindow();
    TestWindow window2 = TestWindow();
    DemoWindow demo = DemoWindow();

    MBIMGUI gui = MBIMGUI("MBIMGUI Test",window);

    if(!gui.Init())
    {
        std::cerr << "Err init" << std::endl;
        return -1;
    }

   // gui.AddChildWindow(&window2);
    gui.AddChildWindow(&demo);

    gui.Show();

    return 0;
}