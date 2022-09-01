#include <filesystem>
#include <cstdarg>
#include <sstream>
#include <ctime>

#include "MBILogger.h"

MBIMGUI::MBILogger::MBILog::MBILog(MBILogLevel level, const std::string &msg) : m_level(level), m_message(msg)
{
    tm ltm;
    time_t now = time(0);
    if (localtime_s(&ltm, &now) == 0)
    {
        std::stringstream sstream;
        sstream << std::setfill('0') << std::setw(2) << ltm.tm_hour
                << ":" << std::setw(2) << ltm.tm_min
                << ":" << std::setw(2) << ltm.tm_sec;
        m_time = sstream.str();
    }
};

MBIMGUI::MBILogLevel MBIMGUI::MBILogger::MBILog::GetLevel() const
{
    return m_level;
}

const std::string MBIMGUI::MBILogger::MBILog::GetLevelString() const
{
    switch (m_level)
    {
    case LOG_LEVEL_WARNING:
        return "WARNING";
    case LOG_LEVEL_ERROR:
        return "ERROR";
    case LOG_LEVEL_DEBUG:
        return "DEBUG";
    default:
    case LOG_LEVEL_INFO:
        return "INFO";
    }
}

const std::string &MBIMGUI::MBILogger::MBILog::GetMessageLog() const
{
    return m_message;
}
const std::string &MBIMGUI::MBILogger::MBILog::GetTime() const
{
    return m_time;
}

void MBIMGUI::MBILogger::CloseLogFile()
{
    if (m_filestream.is_open())
    {
        m_filestream << "*************************** Session End ****************************" << std::endl
                     << std::endl;
        m_filestream.close();
    }
}

MBIMGUI::MBILogger::MBILogger() : m_logs(30), m_logfile(""), m_popupOnError(false), m_displayPopup(false), m_logToFile(false){};

MBIMGUI::MBILogger::~MBILogger()
{
    CloseLogFile();
};

std::string MBIMGUI::MBILogger::GetLogFullFileName() const
{
    if (m_logfile.empty())
    {
        return m_logfile;
    }
    else
    {
        return std::filesystem::canonical(m_logfile).string();
    }
}

void MBIMGUI::MBILogger::Configure(bool popupOnError, const std::string &logfile)
{
    // Close previous openned file
    CloseLogFile();

    if (logfile != "")
    {
        std::ios_base::openmode mode = std::fstream::out;

        if (std::filesystem::exists(logfile))
        {
            // If file is large, truncate it
            if (std::filesystem::file_size(logfile) > 4096)
            {
                mode |= std::fstream::trunc;
            }
            else
            {
                mode |= std::fstream::app;
            }
        }
        else
        {
            mode |= std::fstream::app;
        }
        m_filestream.open(logfile, mode);
        if (m_filestream.fail())
        {
            m_logToFile = false;
            m_logfile = "";
            LogError("Can't open logfile " + logfile);
        }
        else
        {
            tm ltm;
            time_t now = time(0);

            m_filestream << "********************************************************************" << std::endl;
            m_filestream << "************************** Session Start ***************************" << std::endl;

            if (localtime_s(&ltm, &now) == 0)
            {
                m_filestream << std::right << std::setfill('0');
                m_filestream << "**************************   " << std::setw(2) << ltm.tm_mday
                             << "/" << std::setw(2) << (ltm.tm_mon + 1)
                             << "/" << (ltm.tm_year + 1900)
                             << "  ***************************" << std::endl;
                m_filestream << "********************************************************************" << std::endl;
            }
            m_logToFile = true;
            m_logfile = logfile;
        }
    }
    else
    {
        m_logToFile = false;
    }

    m_popupOnError = popupOnError;
}

void MBIMGUI::MBILogger::Log(MBILogLevel level, const std::string &msg)
{
    MBILog log = MBILog(level, msg);
    m_logs.push(log);
    if (level == LOG_LEVEL_ERROR && m_popupOnError == true)
    {
        m_displayPopup = true;
        // Store last error to avoid race condition in case of multiple logs in the same frame
        m_errToPopup = msg;
    }
    if (m_logToFile)
    {
        m_filestream << log.GetLevelString() << "\t" << std::setfill(' ') << std::left << std::setw(50) << log.GetMessageLog() << "\t" << log.GetTime() << std::endl;
    }
}

void MBIMGUI::MBILogger::Log(MBILogLevel level, const char *msg, ...)
{
    va_list args;
    char str[256];

    va_start(args, msg);
    vsnprintf(str, 256, msg, args);
    va_end(args);

    Log(level, std::string(str));
}

void MBIMGUI::MBILogger::LogError(const std::string &msg) { Log(LOG_LEVEL_ERROR, msg); }

void MBIMGUI::MBILogger::PopupError(const std::string &msg)
{
    LogError(msg);
    if (!m_popupOnError)
        m_displayPopup = true;
}