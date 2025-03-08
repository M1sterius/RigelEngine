#pragma once

#include "Core.hpp"
#include "Event.hpp"

/**
 * Contains events that should be inaccessible outside of the engine library
 */

namespace rge::backend
{
    struct TransformUpdateEvent final : public Event
    {
        explicit TransformUpdateEvent() = default;
        ~TransformUpdateEvent() override = default;
    };
}