#include "EventManager.hpp"
#include "Debug.hpp"

namespace rge
{
    EventManager::EventManager() { Startup(); }
    EventManager::~EventManager() { Shutdown(); }

    void EventManager::Startup()
    {
        RGE_TRACE("Starting up event manager.");

        m_Initialized = true;
    }

    void EventManager::Shutdown()
    {
        RGE_TRACE("Shutting down event manager.");
    }
}
