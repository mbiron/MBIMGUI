#pragma once
// Include everything needed for clients
#include "imgui.h"
#include "imfilebrowser.h"
#include "implot.h"

#include "MBILogger.h"

class MBIWindow
{
public:
    enum _MBIWindowConfigFlags
    {
        MBIWindowConfig_hideableInMenu = 1 << 1,
    };
    typedef int MBIWindowConfigFlags;

private:
    ImVec2 m_size;
    std::string m_name;
    bool m_bVisible;
    MBIWindowConfigFlags m_flags;

protected:
    MBILogger &m_logger;

public:
    MBIWindow(std::string name, int height, int width, MBIWindowConfigFlags flags = 0);
    virtual ImVec2 GetWindowSize() const;
    std::string GetName() const;
    virtual void Display() = 0;
    bool IsVisible() const;
    void SetVisible(bool bVisible);
    bool IsInMenu() const;
};