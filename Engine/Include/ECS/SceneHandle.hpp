#pragma once

#include "RigelHandle.hpp"

namespace rge
{
    class Scene;

    class SceneHandle final : public RigelHandle<Scene>
    {
    public:
        NODISCARD bool IsNull() const override;

        // Checks if this scene is correctly registered by the scene manager
        NODISCARD bool IsValid() const override;
    INTERNAL:
        SceneHandle(Scene* ptr, const uid_t id);
    };
}