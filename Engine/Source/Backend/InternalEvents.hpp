#pragma once

#include "Core.hpp"
#include "Event.hpp"

/**
 * Contains events that should be inaccessible outside of the engine library
 */

namespace rge
{
    /**
    * Represents an event fired to process glfw window and input events
    */
    struct PollGlfwEventsEvent final : public Event
    {
        PollGlfwEventsEvent() = default;
    };

    struct InputUpdateEvent final : public Event
    {
        InputUpdateEvent() = default;
    };
}