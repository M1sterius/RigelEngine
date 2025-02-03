#pragma once

#include "RigelHandle.hpp"

namespace rge
{
    class GameObject;

    class GOHandle final : public RigelHandle<GameObject>
    {
    public:
        NODISCARD bool IsNull() const override;

        // Checks if the object is still instantiated on the scene that owns it
        NODISCARD bool IsValid() const override;

        // Returns the ID of the scene this object belongs to
        NODISCARD inline uid_t GetSceneID() const { return m_SceneID; }
    INTERNAL:
        GOHandle(GameObject* ptr, const uid_t id, const uid_t sceneID);
    private:
        uid_t m_SceneID = NULL_ID;
    };
}