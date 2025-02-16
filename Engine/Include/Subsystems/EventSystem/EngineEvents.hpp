#pragma once

#include "Core.hpp"
#include "Event.hpp"

namespace rge
{
    /**
    * Represents a game update event.
    *
    * Dispatched each frame for all subscribed components.
    */
    struct GameUpdateEvent final : public Event
    {
        explicit GameUpdateEvent(const float64_t dt, const uint64_t frameIndex)
            :   deltaTime(dt), frameIndex(frameIndex) {}

        float64_t deltaTime;
        uint64_t frameIndex;
    };
}
