#include "Debug.hpp"
#include "Engine.hpp"

namespace rge
{
    void Debug::Trace(const std::string& message)
    {
        const auto& logger = Logger::Get();
        logger.Log(message, LogType::VerboseMessage);
    }

    void Debug::Message(const std::string& message)
    {
        const auto& logger = Logger::Get();
        logger.Log(message, LogType::Message);
    }

    void Debug::Warning(const std::string& warning)
    {
        const auto& logger = Logger::Get();
        logger.Log(warning, LogType::Warning);
    }

    void Debug::Error(const std::string& error)
    {
        const auto& logger = Logger::Get();
        logger.Log(error, LogType::Error);
    }
}
