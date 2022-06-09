#include "MBIMGUI.h"

MBIWindow::MBIWindow(std::string name, int height, int width, MBIWindowConfigFlags flags) : m_name(name),
                                                                                            m_bVisible(true),
                                                                                            m_flags(flags),
                                                                                            m_logger(MBIMGUI::GetLogger()),
                                                                                            m_size(ImVec2((float)width, (float)height)),
                                                                                            m_imguiFlags(0)
{
}

ImVec2 MBIWindow::GetWindowSize() const
{
    return m_size;
}

const std::string & MBIWindow::GetName() const
{
    return m_name;
}

ImGuiWindowFlags MBIWindow::GetFlags() const
{
    return m_imguiFlags;
}

bool MBIWindow::IsInMenu() const
{
    return (m_flags & MBIWindowConfig_hideableInMenu);
}

bool MBIWindow::IsVisible() const
{
    return m_bVisible;
}

void MBIWindow::SetVisible(bool bVisible)
{
    m_bVisible = bVisible;
}