#include "Debug.hpp"
#include "Engine.hpp"

namespace Rigel
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

    void Debug::Crash(const ErrorCode code, const std::string& message, const char* file, const int line)
    {
        const auto codeCast = static_cast<int32_t>(code);

        Error("Hard crash on line {} in file {}. Crash message: {}. Crash code: {}",
            line, file, message, codeCast);
        std::exit(codeCast);
    }
}
