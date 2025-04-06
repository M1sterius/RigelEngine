#pragma once

#include "RigelHandle.hpp"

namespace rge
{
    class GameObject;

    class GOHandle final : public RigelHandle<GameObject>
    {
    public:
        GOHandle() : RigelHandle(nullptr, NULL_ID) { }
        GOHandle(GameObject* ptr, const uid_t id, const uid_t sceneID);

        NODISCARD inline static GOHandle Null()
        {
            return {nullptr, NULL_ID, NULL_ID};
        }

        NODISCARD bool IsNull() const override;
        NODISCARD bool IsValid() const override;

        // Returns the ID of the scene this object belongs to
        NODISCARD inline uid_t GetSceneID() const { return m_SceneID; }
    private:
        uid_t m_SceneID = NULL_ID;
    };
}