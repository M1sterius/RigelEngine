#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"

namespace Rigel
{
    class Scene;

    class SceneHandle final : public RigelHandle<Scene>
    {
    public:
        NODISCARD const char* GetTypeName() const override
        {
            return "Rigel::SceneHandle";
        }

        SceneHandle() : RigelHandle(nullptr, NULL_ID) { }
        SceneHandle(Scene* ptr, const uid_t id);

        NODISCARD inline static SceneHandle Null()
        {
            return {nullptr, NULL_ID};
        }

        NODISCARD bool IsNull() const override;
        NODISCARD bool IsValid() const override;
    };
}
