#pragma once

#include "RigelHandle.hpp"

namespace Rigel
{
    class GameObject;

    class GOHandle final : public RigelHandle<GameObject>
    {
    public:
        GOHandle() : RigelHandle(nullptr, NULL_ID) { }
        GOHandle(GameObject* ptr, const uid_t id);

        NODISCARD inline static GOHandle Null()
        {
            return {nullptr, NULL_ID};
        }

        NODISCARD bool IsNull() const override;
        NODISCARD bool IsValid() const override;
    };
}
