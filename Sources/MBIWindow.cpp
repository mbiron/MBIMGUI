#include "MBIMGUI.h"

MBIWindow::MBIWindow(std::string name, int height, int width) : m_name(name), m_logger(MBIMGUI::GetLogger()), m_size(ImVec2((float)width, (float)height))
{
}

ImVec2 MBIWindow::GetWindowSize() const
{
    return m_size;
}

std::string MBIWindow::GetName() const
{
    return m_name;
}