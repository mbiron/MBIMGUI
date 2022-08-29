#include "MBIMGUI.h"

MBIMGUI::MBIWindow::MBIWindow(const std::string &name, int height, int width, MBIWindowConfigFlags flags) : m_name(name),
                                                                                             m_bVisible(true),
                                                                                             m_flags(flags),
                                                                                             m_logger(MBIMGUI::GetLogger()),
                                                                                             m_size(ImVec2((float)width, (float)height)),
                                                                                             m_imguiFlags(0)
{
}

ImVec2 MBIMGUI::MBIWindow::GetWindowSize() const
{
    return m_size;
}

const std::string &MBIMGUI::MBIWindow::GetName() const
{
    return m_name;
}

ImGuiWindowFlags MBIMGUI::MBIWindow::GetFlags() const
{
    return m_imguiFlags;
}

bool MBIMGUI::MBIWindow::IsInMenu() const
{
    return (m_flags & MBIWindowConfig_hideableInMenu);
}

bool MBIMGUI::MBIWindow::IsVisible() const
{
    return m_bVisible;
}

void MBIMGUI::MBIWindow::SetVisible(bool bVisible)
{
    m_bVisible = bVisible;
}