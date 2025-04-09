#pragma once

#include "Core.hpp"
#include "Event.hpp"

namespace Rigel::Backend
{
    struct TransformUpdateEvent final : public Event
    {
        explicit TransformUpdateEvent() = default;
        ~TransformUpdateEvent() override = default;
    };

    struct EndOfFrameEvent final : public Event
    {
        explicit EndOfFrameEvent() = default;
        ~EndOfFrameEvent() override = default;
    };
}