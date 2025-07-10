#pragma once

#include "Core.hpp"
#include "Assert.hpp"
#include "Stopwatch.hpp"
#include "Logger.hpp"

#include <string>
#include <format>

namespace Rigel
{
    class Debug
    {
    public:
        template <typename... Args>
        static void Trace(std::format_string<Args...> fmt, Args&&... args)
        {
            const std::string fmtString = std::format(fmt, std::forward<Args>(args)...);
            Trace(fmtString);
        }

        template <typename... Args>
        static void Message(std::format_string<Args...> fmt, Args&&... args)
        {
            const std::string fmtString = std::format(fmt, std::forward<Args>(args)...);
            Message(fmtString);
        }

        template <typename... Args>
        static void Warning(std::format_string<Args...> fmt, Args&&... args)
        {
            const std::string fmtString = std::format(fmt, std::forward<Args>(args)...);
            Warning(fmtString);
        }

        template <typename... Args>
        static void Error(std::format_string<Args...> fmt, Args&&... args)
        {
            const std::string fmtString = std::format(fmt, std::forward<Args>(args)...);
            Error(fmtString);
        }

        static void Crash(const ErrorCode code, const std::string& message, const char* file, const int line);

        static void Trace(const std::string& message);
        static void Message(const std::string& message);
        static void Warning(const std::string& warning);
        static void Error(const std::string& error);
    };
}
