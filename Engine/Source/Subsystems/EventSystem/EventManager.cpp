#include "EventManager.hpp"

#include <iostream>

namespace rge
{
    EventManager::EventManager() { Startup(); }
    EventManager::~EventManager() { Shutdown(); }

    void EventManager::Startup()
    {
        std::cout << "Start up\n";
    }

    void EventManager::Shutdown()
    {
        std::cout << "Shut down\n";
    }
}
