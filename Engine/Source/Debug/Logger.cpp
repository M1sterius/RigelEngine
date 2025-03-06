#include "Logger.hpp"
#include "Directory.hpp"

#include <iomanip>
#include <sstream>
#include <ctime>
#include <iostream>

namespace rge
{
    Logger::Logger()
    {
        m_VisibilityMask = static_cast<uint8_t>(LogType::VerboseMessage) | static_cast<uint8_t>(LogType::Message) |
                           static_cast<uint8_t>(LogType::Warning) | static_cast<uint8_t>(LogType::Error);

        const auto logsDir = Directory::LogsDirectory();

        if (!std::filesystem::exists(logsDir))
            std::filesystem::create_directory(logsDir);

        const auto logsFile = std::filesystem::path(logsDir).concat("/Logs.txt");

        // If an old logs file exists, delete it so that the old logs won't be mixed with new logs
        if (std::filesystem::exists(logsFile))
            std::filesystem::remove(logsFile);

        m_LogsFile = std::make_unique<File>(logsFile, std::ios::out | std::ios::app);
    }

    void Logger::Log(const std::string& log, const LogType type) const
    {
        if (!(m_VisibilityMask & static_cast<uint8_t>(type))) return;

        std::string logTxt = "[" + GetFormattedTime() + " " + GetLogTypePrefix(type) + "] " + log;

        std::cout << GetColorCode(GetLogTypeColor(type)) << logTxt << GetColorCode(ConsoleColor::Default) <<"\n";
        m_LogsFile->WriteText(logTxt + '\n');
        m_LogsFile->Flush();
    }

    void Logger::ChangeLogsVisibilityMask(const LogType type, const bool visibility)
    {
        m_VisibilityMask = visibility ? (m_VisibilityMask | static_cast<uint8_t>(type)) : (m_VisibilityMask & ~static_cast<uint8_t>(type));
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

        return oss.str();
    }

    const char* Logger::GetColorCode(const ConsoleColor color)
    {
        switch (color)
        {
            case ConsoleColor::Green: return CONSOLE_COLOR_GREEN;
            case ConsoleColor::Yellow: return CONSOLE_COLOR_YELLOW;
            case ConsoleColor::Red: return CONSOLE_COLOR_RED;
            default: return CONSOLE_COLOR_DEFAULT;
        }
    }

    ConsoleColor Logger::GetLogTypeColor(const LogType type)
    {
        switch (type)
        {
            case LogType::VerboseMessage: return ConsoleColor::Green;
            case LogType::Message: return ConsoleColor::Default;
            case LogType::Warning: return ConsoleColor::Yellow;
            case LogType::Error: return ConsoleColor::Red;
        }

        return ConsoleColor::Default;
    }

    const char* Logger::GetLogTypePrefix(const LogType type)
    {
        switch (type)
        {
            case LogType::VerboseMessage: return "VMSG";
            case LogType::Message: return "MSG";
            case LogType::Warning: return "WARN";
            case LogType::Error: return "ERR";
        }

        return "Invalid";
    }
}
