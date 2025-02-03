#pragma once

#include "HeaderUtils.hpp"
#include "RigelHandle.hpp"
#include "Component.hpp"

#include <string>

namespace rge
{
    class RGE_API GameObject final
    {
    public:
        GameObject(const GameObject&) = delete;
        GameObject operator = (const GameObject&) = delete;

        NODISCARD inline uid_t GetID() const { return m_ID; }
        NODISCARD inline uid_t GetSceneID() const { return m_SceneID; }
    private:
        explicit GameObject(std::string name);
        ~GameObject();

        uid_t m_ID = NULL_ID;
        uid_t m_SceneID = NULL_ID;
        std::string m_Name;

        friend class Scene;
    };

    class RGE_API GOHandle final : public RigelHandle<GameObject>
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
