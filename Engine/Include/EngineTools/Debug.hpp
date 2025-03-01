#pragma once

#include "Core.hpp"
#include "Assert.hpp"
#include "Stopwatch.hpp"

#ifdef RIGEL_INTERNAL
    #include "Logger.hpp"
#endif

#include <string>

namespace rge
{
    class Debug
    {
    public:
        static void VerboseMessage(const std::string& message);
        static void Message(const std::string& message);
        static void Warning(const std::string& warning);
        static void Error(const std::string& error);
    };
}
