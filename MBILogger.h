#pragma once

#include <fstream>

#include "MBISyncCircularBuffer.h"

namespace MBIMGUI
{

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

    /**
     * @brief This class represent the logger of the application. The logger allows you to display debug and operational
     * logs to the users, to write them into a logfile, to display error popup etc.
     *
     */
    class MBILogger
    {
    public:
    private:
        /**
         * @brief Internal class to represent a log element
         *
         */
        class MBILog
        {
        public:
            /**
             * @brief Construct a new MBILog object
             *
             * @param level Level of the log
             * @param msg Message of the log
             */
            MBILog(MBILogLevel level = LOG_LEVEL_INFO, std::string msg = "");

            /**
             * @brief Get the Log level as an object
             *
             * @return MBILogLevel level of the log
             */
            MBILogLevel GetLevel() const;
            /**
             * @brief Get the Log level as a string
             *
             * @return const std::string level of the log
             */
            const std::string GetLevelString() const;
            /**
             * @brief Get the Message Log object as a string
             *
             * @return const std::string& message of the log
             */
            const std::string &GetMessageLog() const;
            /**
             * @brief Get the date when the log has been raised.
             *
             * @return const std::string& time as a string
             */
            const std::string &GetTime() const;

        private:
            MBILogLevel m_level;   ///< Level of the log
            std::string m_message; ///< Message of the log
            std::string m_time;    ///< Date of the log
        };

        MBISyncCircularBuffer<MBILog> m_logs; ///< List of the current logs
        friend class MBILogWindow;
        std::string m_logfile;      ///< Path of the current logfile
        std::ofstream m_filestream; ///< Stream used to write into the logfile
        std::string m_errToPopup;   ///< Last error raised, to be displayed in the popup

        bool m_popupOnError; ///< Is the logger must display a popup in case of an error ?
        bool m_displayPopup; ///< True if a popup must be displayed. It means that an error has been raised in the previous frame
        bool m_logToFile;    ///< Is the logger must write the logs into a file ?

        /**
         * @brief Close the current logfile.
         *
         */
        void CloseLogFile();

    public:
        /**
         * @brief Construct a new MBILogger object
         * @warning You souhld never call this, the logger of the app is created internally by the framework. You can reterive it using @ref MBIMGUI::GetLogger()
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
        /**
         * @brief Log a message
         *
         * @param level Level of the message
         * @param msg Message to be logged
         * @param ... Varargs
         */
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
         * The popup will appear even if the @ref Configure function has been used to disable automatic popup on error.
         *
         * @param msg Message to be logged and displayed in the popup
         */
        void PopupError(std::string &msg);
    };
}