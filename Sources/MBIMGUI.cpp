#include "MBIMGUI.h"

MBILogger &MBIMGUI::GetLogger()
{
    static MBILogger logger = MBILogger();
    return logger;
}



