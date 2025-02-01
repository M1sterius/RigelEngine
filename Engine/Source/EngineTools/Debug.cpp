#include "Debug.hpp"
#include "Engine.hpp"
#include "../Backend/Logger/Logger.hpp"

namespace rge
{
    void Debug::VerboseMessage(const std::string& message)
    {
        const auto& logger = Engine::Get().GetLogger();
        logger.VerboseMessage(message);
    }

    void Debug::Message(const std::string& message)
    {
        const auto& logger = Engine::Get().GetLogger();
        logger.Message(message);
    }

    void Debug::Warning(const std::string& warning)
    {
        const auto& logger = Engine::Get().GetLogger();
        logger.Warning(warning);
    }

    void Debug::Error(const std::string& error)
    {
        const auto& logger = Engine::Get().GetLogger();
        logger.Error(error);
    }
}
