#include "MBIMGUI.h"

MBIMGUI::MBIWindow::MBIWindow(std::string_view name, int height, int width, MBIWindowConfigFlags flags) : m_name(name),
                                                                                                            m_bVisible(true),
                                                                                                            m_flags(flags),
                                                                                                            m_logger(MBIMGUI::GetLogger()),
                                                                                                            m_size(ImVec2((float)width, (float)height)),
                                                                                                            m_imguiFlags(0)
{
}

ImVec2 MBIMGUI::MBIWindow::GetWindowSize() const noexcept
{
    return m_size;
}

ImVec2 MBIMGUI::MBIWindow::GetWindowPos() const noexcept
{
    return m_pos;
}

const std::string &MBIMGUI::MBIWindow::GetName() const noexcept
{
    return m_name;
}

ImGuiWindowFlags MBIMGUI::MBIWindow::GetFlags() const noexcept
{
    return m_imguiFlags;
}

bool MBIMGUI::MBIWindow::IsInMenu() const noexcept
{
    return (m_flags & MBIWindowConfig_hideableInMenu);
}

bool MBIMGUI::MBIWindow::IsVisible() const noexcept
{
    return m_bVisible;
}

void MBIMGUI::MBIWindow::SetVisible(bool bVisible) noexcept
{
    m_bVisible = bVisible;
}

void MBIMGUI::MBIWindow::UpdateAndDisplay()
{
    m_size = ImGui::GetWindowSize();
    m_pos = ImGui::GetWindowPos();
    Display();
}