# MBIMGUI

Small overlay above the amazing [DearImGUI](https://github.com/ocornut/imgui/).
This library packages imgui and few useful widgets/extensions.

This lib offers a simple API to create a desktop app based on %ImGui (main API in MBIMGUI.h).
To create a basic GUI application simply extend an MBIMGUI::MBIWindow class and implement the Display() method, calling %ImGui fnuctions directly.
The MBIMGUI lib handles all the rendering (DX12 only for now) and presetting of the %ImGui framework.

The lib provides useful services for basic applications, like a logger mechanism (MBIMGUI::MBILogger), a persistent option API (MBIMGUI::MBIOption), a filebrowser.
Two classes implements [ImPlot](https://github.com/epezent/implot/) providing generics plots objects ready to use (MBIPlotChart and MBIRealtimePlotChart).


# Example

A basic usage example is available in ./example directory. 

# Documentation

Framework documentation is available in ./doc directory.