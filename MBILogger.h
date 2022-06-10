#pragma once

#include <fstream>

#include "MBICircularBuffer.h"

/**
 * @brief This class represent the logger of the application. The logger allows you to display debug and operational
 * logs to the users, to write them into a logfile, to display error popup etc.
 *
 */
class MBILogger
{
public:
    /**
     * @brief Level of the log
     *
     */
    typedef enum _MBILogLevel
    {
        LOG_LEVEL_INFO,    ///< Nominal info log
        LOG_LEVEL_WARNING, ///< Warning log
        LOG_LEVEL_ERROR,   ///< Error log
        LOG_LEVEL_DEBUG    ///< Debug log
    } MBILogLevel;

private:
    /**
     * @brief Internal class to represent a log element
     *
     */
    class MBILog
    {
    public:
        MBILog(MBILogLevel level = LOG_LEVEL_INFO, std::string msg = "");

        MBILogLevel GetLevel() const;
        const std::string GetLevelString() const;
        const std::string &GetMessageLog() const;
        const std::string &GetTime() const;

    private:
        MBILogLevel m_level;
        std::string m_message;
        std::string m_time;
    };

    MBICircularBuffer<MBILog> m_logs;
    friend class MBILogWindow;
    std::string m_logfile;
    std::ofstream m_filestream;
    std::string m_errToPopup;

    bool m_popupOnError;
    bool m_displayPopup;
    bool m_logToFile;

    void CloseLogFile();

public:
    /**
     * @brief Construct a new MBILogger object
     * @warning You souhld never call this, the logger of the app is created internally by the framework. You can reterive it using MBIMGUI::GetLogger()
     */
    MBILogger();

    /**
     * @brief Destroy the MBILogger object
     *
     */
    ~MBILogger();

    /**
     * @brief Get the Log Full File Name if a log file was setted.
     *
     * @return std::string containing the path to the logfile or an empty string if no log file was specified.
     */
    std::string GetLogFullFileName() const;

    /**
     * @brief Configure the logger behaviour.
     *
     * @param popupOnError Any error log will be displayed in a popup to the user.
     * @param logfile All logs will be written in the specified logfile. To disable the log file, pass an empty string.
     */
    void Configure(bool popupOnError = false, const std::string &logfile = "");

    /**
     * @brief Log a message
     *
     * @param level Level of the message
     * @param msg Message to be logged
     */
    void Log(MBILogLevel level, std::string &msg);
    void Log(MBILogLevel level, const char *msg, ...);
    /**
     * @brief Log a message with the level MBILogLevel::LOG_LEVEL_ERROR
     *
     * @param msg Message to be logged
     */
    void LogError(std::string &msg);

    // TODO : LogInfo ? LogOnce ! May be useful for debug

    /**
     * @brief Log a message with the level MBILogLevel::LOG_LEVEL_ERROR and display it in a popup.
     * The popup will appear even if the ::Configure function has been used to disable automatic popup on error.
     *
     * @param msg Message to be logged and displayed in the popup
     */
    void PopupError(std::string &msg);
};