#pragma once

#include <string>
#include <ctime>
#include <vector>
//#include "MBILogWindow.h"
// class MBILogWindow;

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
        MBILog(MBILogLevel level, std::string msg) : m_level(level), m_message(msg)
        {
            time_t now = time(0);
            tm *ltm = localtime(&now);
            m_time = std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec);
        };

        std::string GetLevel() const
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

    // TODO : use a circular buffer ?
    std::vector<MBILog> m_logs;
    friend class MBILogWindow;

public:
    MBILogger()
    {
        m_logs.reserve(20);
    };

    void Log(MBILogLevel level, std::string msg) { m_logs.push_back(MBILog(level, msg)); }
    void LogInfo( std::string msg) { Log(LOG_LEVEL_INFO, msg); }

};