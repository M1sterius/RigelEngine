#pragma once

#include "HeaderUtils.hpp"
#include "RigelSubsystem.hpp"
#include "IEventListener.hpp"

namespace rge
{
    class EventManager : public RigelSubsystem
    {
    INTERNAL:
        EventManager();
        ~EventManager();
    private:
        void Startup() override;
        void Shutdown() override;
    };
}
