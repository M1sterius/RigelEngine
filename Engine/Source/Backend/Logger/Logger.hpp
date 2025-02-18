#pragma once

#include "Core.hpp"

#include <cstdint>
#include <string>
#include <stdexcept>

namespace rge
{
    enum class LogType : uint8_t
    {
        VerboseMessage = 1,
        Message = 2,
        Warning = 4,
        Error = 8
    };

    enum class ConsoleColor
    {
        Default,
        Green,
        Yellow,
        Red
    };

    class Logger
    {
    public:
        inline static Logger& Get()
        {
            static auto instance = Logger();
            return instance;
        }

        void Log(const std::string& log, const LogType type) const;

        void ChangeLogsVisibilityMask(const LogType type, const bool visibility);
        NODISCARD inline uint8_t GetVisibilityMask() const { return m_VisibilityMask; }
    private:
        Logger();
        ~Logger() = default;

        uint8_t m_VisibilityMask;

        NODISCARD static std::string GetFormattedTime();
        NODISCARD static const char* GetColorCode(const ConsoleColor color);
        static ConsoleColor GetLogTypeColor(const LogType type);
        static const char* GetLogTypePrefix(const LogType type);

        inline static const char* CONSOLE_COLOR_DEFAULT = "\033[0m";
        inline static const char* CONSOLE_COLOR_GREEN = "\033[32m";
        inline static const char* CONSOLE_COLOR_YELLOW = "\033[33m";
        inline static const char* CONSOLE_COLOR_RED = "\033[31m";
    };

/**
* A macro that helps to throw std::runtime_error while simultaneously
* properly logging it via rge::Logger
*/
#define THROW_RUNTIME_ERROR(Text) \
    rge::Logger::Get().Log(Text, rge::LogType::Error); \
    throw std::runtime_error(Text)

#ifdef DEBUG_BUILD
    #define VERBOSE_MESSAGE(message) rge::Logger::Get().Log(message, LogType::VerboseMessage)
#else
    #define VERBOSE_MESSAGE(message)
#endif

}