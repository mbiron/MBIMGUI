#include <iostream>
#include "MBIMGUI.h"
#include "TestWindow.h"


int main(int argc, char **argv)
{
    TestWindow window = TestWindow();
    TestWindow window2 = TestWindow();

    MBIMGUI gui = MBIMGUI("MBIMGUI Test",window);

    if(!gui.Init())
    {
        std::cerr << "Err init" << std::endl;
        return -1;
    }

    gui.AddChildWindow(&window2);

    gui.Show();

    return 0;
}