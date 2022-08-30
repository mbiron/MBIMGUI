# MBIMGUI

Small overlay above the amazing [DearImGUI](https://github.com/ocornut/imgui/).
This library package imgui and few useful widgets/extensions.

This lib offers a simple API to create a desktop app based on %ImGui (main API in MBIMGUI.h).
The main idea for now is to extend an MBIWindow and implement the Display() method by calling %ImGui primitives.
The MBIMGUI lib handles all the rendering (DX12 only for now) and presetting of the %ImGui framework.

# Example

A basic usage example is available in ./example directory. 

# Documentation

Framework documentation is available in ./doc directory.