#include "EventManager.hpp"
#include "Debug.hpp"

namespace rge
{
    EventManager::EventManager() { Startup(); }
    EventManager::~EventManager() { Shutdown(); }

    void EventManager::Startup()
    {
        Debug::Trace("Starting up event manager.");

        m_Initialized = true;
    }

    void EventManager::Shutdown()
    {
        Debug::Trace("Shutting down event manager.");
    }
}
