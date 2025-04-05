#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"

namespace rge
{
    class Scene;

    class SceneHandle final : public RigelHandle<Scene>
    {
    public:
        SceneHandle(Scene* ptr, const uid_t id);

        NODISCARD inline static SceneHandle Null()
        {
            return {nullptr, NULL_ID};
        }

        NODISCARD bool IsNull() const override;
        NODISCARD bool IsValid() const override;
    };
}
