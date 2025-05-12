#pragma once

#include "Core.hpp"
#include "Event.hpp"
#include "../InputManager/KeysAndButtons.hpp"
#include "Math.hpp"

namespace Rigel
{
    /**
    * Represents a game update event.
    *
    * Dispatched each frame for all subscribed components.
    */
    struct GameUpdateEvent final : public Event
    {
        explicit GameUpdateEvent(const float64_t dt, const uint64_t frameIndex)
            : DeltaTime(dt), FrameIndex(frameIndex) {}

        float64_t DeltaTime;
        uint64_t FrameIndex;
    };

    /**
     * Represents a physics update/tick event.
     *
     * Dispatched fixed number of times per second according to the desired Tickrate setting
     */
    struct PhysicsTickEvent final : public Event
    {
        explicit PhysicsTickEvent(const float64_t dt) : TickTime(dt) { }

        float64_t TickTime;
    };

    /**
    * Represents an event fired when the window is being resized
    */
    struct WindowResizeEvent final : public Event
    {
        explicit WindowResizeEvent(const glm::uvec2& newSize)
            : NewSize(newSize) { }

        glm::uvec2 NewSize;
    };

    struct KeyDownEvent final : public Event
    {
        explicit KeyDownEvent(const KeyCode key) : Key(key) { }

        KeyCode Key;
    };

    struct KeyUpEvent final : public Event
    {
        explicit KeyUpEvent(const KeyCode key) : Key(key) { }

        KeyCode Key;
    };

    struct KeyPressedEvent final : public Event
    {
        explicit KeyPressedEvent(const KeyCode key) : Key(key) { }

        KeyCode Key;
    };

    struct MouseMoveEvent final : public Event
    {
        MouseMoveEvent(const glm::vec2 pos, const glm::vec2 delta) : Position(pos), Delta(delta) { }

        glm::vec2 Position;
        glm::vec2 Delta;
    };

    struct MouseScrollEvent final : public Event
    {
        explicit MouseScrollEvent(const glm::vec2 offset) : ScrollOffset(offset) { }

        glm::vec2 ScrollOffset;
    };

    struct MouseButtonDownEvent final : public Event
    {
        explicit MouseButtonDownEvent(const MouseButton button) : Button(button) { }

        MouseButton Button;
    };

    struct MouseButtonPressedEvent final : public Event
    {
        explicit MouseButtonPressedEvent(const MouseButton button) : Button(button) { }

        MouseButton Button;
    };

    struct MouseButtonUpEvent final : public Event
    {
        explicit MouseButtonUpEvent(const MouseButton button) : Button(button) { }

        MouseButton Button;
    };
}
