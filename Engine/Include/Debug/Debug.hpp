#pragma once

#include "Core.hpp"
#include "Assert.hpp"
#include "Stopwatch.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"

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

        /**
         * Helps to throw an instance of RigelError exception with proper logging
         * @param errorText A message explaining the nature of the error
         */
        inline static void ThrowError(const std::string& errorText)
        {
            Error(errorText);
            throw RigelError(errorText);
        }
    };
}
