#pragma once

#include "../Utils/HeaderUtils.hpp"

#include <string>

namespace rge
{
    class RGE_API Debug
    {
    public:
        static void VerboseMessage(const std::string& message);
        static void Message(const std::string& message);
        static void Warning(const std::string& warning);
        static void Error(const std::string& error);
    };
}
