#include "EventManager.hpp"
#include "Debug.hpp"

namespace Rigel
{
    int32_t EventManager::Startup()
    {
        Debug::Trace("Starting up event manager.");

        m_Initialized = true;
        return 0;
    }

    int32_t EventManager::Shutdown()
    {
        Debug::Trace("Shutting down event manager.");
        return 0;
    }
}
