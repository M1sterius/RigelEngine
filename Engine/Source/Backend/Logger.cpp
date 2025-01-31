#include "Logger.hpp"

#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iostream>

namespace rge
{
    const char* Logger::CONSOLE_COLOR_DEFAULT = "\033[0m";
    const char* Logger::CONSOLE_COLOR_GREEN = "\033[32m";
    const char* Logger::CONSOLE_COLOR_YELLOW = "\033[33m";
    const char* Logger::CONSOLE_COLOR_RED = "\033[31m";

    Logger::Logger()
        : m_VisibilityMask(LOG_TYPE::LOG_MESSAGE | LOG_TYPE::LOG_WARNING | LOG_TYPE::LOG_ERROR | LOG_TYPE::LOG_VERBOSE_MESSAGE)
    {

    }

    Logger::~Logger() = default;

    void Logger::Log(const std::string& log, const LOG_TYPE type) const
    {
        if (!(m_VisibilityMask & type)) return;

        if (EnableConsoleColors) ChangeConsoleTextColor(GetColorCode(type));
        printf("%s %s\n", GetFormattedTime().c_str(), log.c_str());
        if (EnableConsoleColors) ChangeConsoleTextColor(CONSOLE_COLOR_DEFAULT);
    }

    void Logger::VerboseMessage(const std::string& message)
    {
        Log(message, LOG_TYPE::LOG_VERBOSE_MESSAGE);
    }

    void Logger::Message(const std::string& message) const
    {
        Log(message, LOG_TYPE::LOG_MESSAGE);
    }

    void Logger::Warning(const std::string& warning) const
    {
        Log(warning, LOG_TYPE::LOG_WARNING);
    }

    void Logger::Error(const std::string& error) const
    {
        Log(error, LOG_TYPE::LOG_ERROR);
    }

    void Logger::ChangeLogsVisibilityMask(const LOG_TYPE type, const bool visibility)
    {
        m_VisibilityMask = visibility ? (m_VisibilityMask | type) : (m_VisibilityMask & ~type);
    }

    std::string Logger::GetFormattedTime()
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t time_now = std::chrono::system_clock::to_time_t(now);

        std::tm local_tm = *std::localtime(&time_now);

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << local_tm.tm_hour << ":"
            << std::setw(2) << std::setfill('0') << local_tm.tm_min << ":"
            << std::setw(2) << std::setfill('0') << local_tm.tm_sec;

        return "[" + oss.str() + "]";
    }

    const char* Logger::GetColorCode(const LOG_TYPE type)
    {
        switch (type)
        {
            case LOG_MESSAGE: return CONSOLE_COLOR_DEFAULT;
            case LOG_WARNING: return CONSOLE_COLOR_YELLOW;
            case LOG_ERROR: return CONSOLE_COLOR_RED;
            case LOG_VERBOSE_MESSAGE: return CONSOLE_COLOR_GREEN;
            default: return CONSOLE_COLOR_DEFAULT;
        }
    }

    void Logger::ChangeConsoleTextColor(const char* colorCode)
    {
        std::cout << colorCode;
    }
}
