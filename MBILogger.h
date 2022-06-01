#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "MBICircularBuffer.h"

class MBILogger
{
public:
    typedef enum _MBILogLevel
    {
        LOG_LEVEL_INFO,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_DEBUG
    } MBILogLevel;

private:
    class MBILog
    {
    public:
        MBILog(MBILogLevel level = LOG_LEVEL_INFO, std::string msg = "") : m_level(level), m_message(msg)
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

        MBILogLevel GetLevel() const
        {
            return m_level;
        }

        std::string GetLevelString() const
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

        std::string GetMessage() const { return m_message; }
        std::string GetTime() const { return m_time; }

    private:
        MBILogLevel m_level;
        std::string m_message;
        std::string m_time;
    };

    MBICircularBuffer<MBILog> m_logs;
    friend class MBILogWindow;
    std::string m_logfile;
    std::ofstream m_filestream;

    bool m_popupOnError;
    bool m_displayPopup;

public:
    MBILogger() : m_logs(30), m_logfile(""), m_popupOnError(false), m_displayPopup(false){};
    ~MBILogger()
    {
        m_filestream << "*************************** Session End ****************************" << std::endl
                     << std::endl;
        m_filestream.close();
    };
    void ConfigureLogs(bool popupOnError = false, const std::string logfile = "")
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
        m_popupOnError = popupOnError;
    }
    void Log(MBILogLevel level, std::string msg)
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
    void LogInfo(std::string msg) { Log(LOG_LEVEL_INFO, msg); }
    void LogError(std::string msg) { Log(LOG_LEVEL_ERROR, msg); }
};