#pragma once

#include <string>
#include <ctime>
#include <vector>
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
                // TODO : handle format properly (0 padding when value < 10 )
                m_time = std::to_string(ltm.tm_hour) + ":" + std::to_string(ltm.tm_min) + ":" + std::to_string(ltm.tm_sec);
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

public:
    MBILogger() : m_logs(30){};

    void Log(MBILogLevel level, std::string msg) { m_logs.push(MBILog(level, msg)); }
    void LogInfo(std::string msg) { Log(LOG_LEVEL_INFO, msg); }
};