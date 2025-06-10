#include "EventManager.hpp"
#include "Debug.hpp"

namespace Rigel
{
    ErrorCode EventManager::Startup(const ProjectSettings& settings)
    {
        Debug::Trace("Starting up event manager.");

        m_Initialized = true;
        return ErrorCode::OK;
    }

    ErrorCode EventManager::Shutdown()
    {
        Debug::Trace("Shutting down event manager.");
        return ErrorCode::OK;
    }
}
