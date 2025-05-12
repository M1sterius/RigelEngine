#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "KeysAndButtons.hpp"

namespace Rigel
{
    class Input
    {
    public:
        NODISCARD static bool GetKeyDown(const KeyCode key);
        NODISCARD static bool GetKeyPressed(const KeyCode key);
        NODISCARD static bool GetKeyUp(const KeyCode key);

        NODISCARD static bool GetMouseButtonDown(const MouseButton button);
        NODISCARD static bool GetMouseButtonPressed(const MouseButton button);
        NODISCARD static bool GetMouseButtonUp(const MouseButton button);

        NODISCARD static glm::vec2 GetMousePosition();
        NODISCARD static glm::vec2 GetMouseDelta();
    };
}