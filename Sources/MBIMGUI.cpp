#include "MBIMGUI.h"

MBIMGUI::MBILogger &MBIMGUI::GetLogger()
{
    static MBILogger logger = MBILogger();
    return logger;
}



