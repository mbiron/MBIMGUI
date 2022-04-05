#include <iostream>
#include "MBIMGUI.h"

void MBIHMI(void *arg)
{
    static float f = 0.0f;
    static int counter = 0;
    static bool show_demo_window = false;
    static bool show_another_window = false;

    ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f

    if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

int main(int argc, char **argv)
{
    MBIMGUI gui = MBIMGUI("MBIMGUI Test",&MBIHMI,NULL, 600 ,400);

    if(!gui.Init())
    {
        std::cerr << "Err init" << std::endl;
        return -1;
    }

    gui.Display();

    return 0;
}