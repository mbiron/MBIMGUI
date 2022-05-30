#pragma once
// Include everything needed for clients
#include "imgui.h"
#include "imfilebrowser.h"
#include "implot.h"

#include "MBILogger.h"

class MBIWindow
{
private:
    ImVec2 m_size;
    std::string m_name;

protected:
    MBILogger &m_logger;

public:
    MBIWindow(std::string name, int height, int width);
    virtual ImVec2 GetWindowSize() const;
    std::string GetName() const;
    virtual void Display() = 0;
};