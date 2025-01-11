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

    bool Logger::EnableConsoleColors = true;
    uint8_t Logger::VisibilityMask = LOG_TYPE::VERBOSE_MESSAGE | LOG_TYPE::MESSAGE | LOG_TYPE::WARNING | LOG_TYPE::ERROR;

    void Logger::Log(const std::string& log, const LOG_TYPE type)
    {
        if (!(VisibilityMask & type)) return;

        if (EnableConsoleColors) ChangeConsoleTextColor(GetColorCode(type));
        printf("%s %s\n", GetFormattedTime().c_str(), log.c_str());
        if (EnableConsoleColors) ChangeConsoleTextColor(CONSOLE_COLOR_DEFAULT);
    }

    void Logger::VerboseMessage(const std::string& message)
    {
        #ifdef DEBUG_BUILD
        Log(message, LOG_TYPE::VERBOSE_MESSAGE);
        #endif
    }

    void Logger::Message(const std::string& message)
    {
        #ifdef DEBUG_BUILD
        Log(message, LOG_TYPE::MESSAGE);
        #endif
    }

    void Logger::Warning(const std::string& warning)
    {
        #ifdef DEBUG_BUILD
        Log(warning, LOG_TYPE::WARNING);
        #endif
    }

    void Logger::Error(const std::string& error)
    {
        #ifndef RIGEL_DISABLE_ERROR_LOGS_IN_NON_DEBUG
        Log(error, LOG_TYPE::ERROR);
        #endif
    }

    void Logger::ChangeLogsVisibilityMask(const LOG_TYPE type, const bool visibility)
    {
        VisibilityMask = visibility ? (VisibilityMask | type) : (VisibilityMask & ~type);
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
            case VERBOSE_MESSAGE: return CONSOLE_COLOR_GREEN;
            case MESSAGE: return CONSOLE_COLOR_DEFAULT;
            case WARNING: return CONSOLE_COLOR_YELLOW;
            case ERROR: return CONSOLE_COLOR_RED;
            default: return CONSOLE_COLOR_DEFAULT;
        }
    }

    void Logger::ChangeConsoleTextColor(const char* colorCode)
    {
        std::cout << colorCode;
    }

    void Logger::SetConsoleColorsVisibility(const bool visibility)
    {
        EnableConsoleColors = visibility;
    }
}
