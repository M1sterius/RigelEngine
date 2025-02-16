#include "EventManager.hpp"
#include "Logger.hpp"

namespace rge
{
    EventManager::EventManager() { Startup(); }
    EventManager::~EventManager() { Shutdown(); }

    void EventManager::Startup()
    {
        VERBOSE_MESSAGE("Starting up event manager.");
    }

    void EventManager::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down event manager.");
    }
}
