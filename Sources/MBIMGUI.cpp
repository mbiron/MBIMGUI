

#include "MBIMGUI.h"



MBIMGUI::MBILogger &MBIMGUI::GetLogger() noexcept
{
    static MBILogger logger = MBILogger();
    return logger;
}
