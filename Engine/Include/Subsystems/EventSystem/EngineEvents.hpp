#pragma once

#include "Core.hpp"
#include "Event.hpp"
#include "KeysAndButtons.hpp"
#include "glm.hpp"

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

    /**
    * Represents an event fired to process glfw window and input events
    */
    struct PollGlfwEventsEvent final : public Event
    {
        PollGlfwEventsEvent() = default;
    };

    /**
    * Represents an event fired when the window is being resized
    */
    struct WindowResizeEvent final : public Event
    {
        explicit WindowResizeEvent(const glm::uvec2& newSize)
            :   NewSize(newSize) { }

        glm::uvec2 NewSize;
    };

    struct OnKeyDownEvent final : public Event
    {
        explicit OnKeyDownEvent(const KeyCode key) : Key(key) { }

        KeyCode Key;
    };

    struct OnKeyUpEvent final : public Event
    {
        explicit OnKeyUpEvent(const KeyCode key) : Key(key) { }

        KeyCode Key;
    };

    struct OnKeyPressedEvent final : public Event
    {
        explicit OnKeyPressedEvent(const KeyCode key) : Key(key) { }

        KeyCode Key;
    };
}
