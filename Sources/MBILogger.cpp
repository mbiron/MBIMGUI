#include <filesystem>
#include "MBILogger.h"

MBILogger::MBILog::MBILog(MBILogLevel level, std::string msg) : m_level(level), m_message(msg)
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

MBILogger::MBILogLevel MBILogger::MBILog::GetLevel() const
{
    return m_level;
}

std::string MBILogger::MBILog::GetLevelString() const
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

std::string MBILogger::MBILog::GetMessage() const
{
    return m_message;
}
std::string MBILogger::MBILog::GetTime() const
{
    return m_time;
}

/**
 * @brief Construct a new MBILogger object
 * @warning You never have to called this, the logger of the app is created internally by the framework. You can reterive it using MBIMGUI::GetLogger()
 */
MBILogger::MBILogger() : m_logs(30), m_logfile(""), m_popupOnError(false), m_displayPopup(false){};
/**
 * @brief Destroy the MBILogger object
 *
 */
MBILogger::~MBILogger()
{
    m_filestream << "*************************** Session End ****************************" << std::endl
                 << std::endl;
    m_filestream.close();
};
/**
 * @brief Get the Log Full File Name if a log file was setted.
 *
 * @return std::string containing the path to the logfile or an empty string if no log file was specified.
 */
std::string MBILogger::GetLogFullFileName() const
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
/**
 * @brief Configure the logger behaviour.
 *
 * @param popupOnError Any error log will be displayed in a popup to the user.
 * @param logfile All logs will be written in the specified logfile. To disable the log file, pass an empty string.
 */
void MBILogger::Configure(bool popupOnError, const std::string logfile)
{
    if (logfile != "")
    {
        m_filestream.open(logfile, std::fstream::out | std::fstream::app);
        if (m_filestream.fail())
        {
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
                m_filestream << std::setfill('0');
                m_filestream << "**************************   " << std::setw(2) << ltm.tm_mday
                             << "/" << std::setw(2) << (ltm.tm_mon + 1)
                             << "/" << (ltm.tm_year + 1900)
                             << "  ***************************" << std::endl;
                m_filestream << "********************************************************************" << std::endl;
            }
            m_logfile = logfile;
        }
    }
    else
    {
        if (m_filestream.is_open())
        {
            m_filestream.close();
        }
        m_logfile = logfile;
    }
    m_popupOnError = popupOnError;
}
/**
 * @brief Log a message
 *
 * @param level Level of the message
 * @param msg Message to be logged
 */
void MBILogger::Log(MBILogLevel level, std::string msg)
{
    MBILog log = MBILog(level, msg);
    m_logs.push(log);
    if (level == LOG_LEVEL_ERROR && m_popupOnError == true)
    {
        m_displayPopup = true;
    }
    if (m_filestream.is_open())
    {
        m_filestream << log.GetLevelString() << "\t" << std::setfill(' ') << std::left << std::setw(50) << log.GetMessage() << "\t" << log.GetTime() << std::endl;
    }
}
/**
 * @brief Log a message with the level MBILogLevel::LOG_LEVEL_INFO
 *
 * @param msg Message to be logged
 */
void MBILogger::LogInfo(std::string msg) { Log(LOG_LEVEL_INFO, msg); }
/**
 * @brief Log a message with the level MBILogLevel::LOG_LEVEL_ERROR
 *
 * @param msg Message to be logged
 */
void MBILogger::LogError(std::string msg) { Log(LOG_LEVEL_ERROR, msg); }
/**
 * @brief Log a message with the level MBILogLevel::LOG_LEVEL_ERROR and display it in a popup.
 * The popup will appear even if the ::Configure function has been used to disable automatic popup on error.
 *
 * @param msg Message to be logged and displayed in the popup
 */
void MBILogger::PopupError(std::string msg)
{
    LogError(msg);
    if (!m_popupOnError)
        m_displayPopup = true;
}