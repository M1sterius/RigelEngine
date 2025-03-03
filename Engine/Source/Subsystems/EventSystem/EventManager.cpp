#include "EventManager.hpp"
#include "Logger.hpp"

namespace rge
{
    EventManager::EventManager() { Startup(); }
    EventManager::~EventManager() { Shutdown(); }

    void EventManager::Startup()
    {
        RGE_TRACE("Starting up event manager.");
    }

    void EventManager::Shutdown()
    {
        RGE_TRACE("Shutting down event manager.");
    }
}
