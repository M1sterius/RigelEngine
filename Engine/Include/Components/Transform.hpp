#pragma once

#include "Core.hpp"
#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"
#include "Math.hpp"

#include <iostream>

namespace rge
{
    class Transform final : public Component
    {
    public:
        REGISTER_COMPONENT(Transform);

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetScale(const glm::vec3& scale);

        NODISCARD glm::vec3 GetPosition() const { return m_Position; }
        NODISCARD glm::quat GetRotation() const { return m_Rotation; }
        NODISCARD glm::vec3 GetScale() const { return m_Scale; }

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;
    private:
        Transform();
        Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

        void OnStart() override;
        void OnDestroy() override;

        void Update();

        glm::vec3 m_Position;
        glm::quat m_Rotation;
        glm::vec3 m_Scale;

        // No serialization intended
        bool m_UpdateRequiredFlag = true;
        uid_t m_UpdateCallbackID = NULL_ID;

        friend class GameObject;
    };
}
