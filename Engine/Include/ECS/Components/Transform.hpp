#pragma once

#include "Core.hpp"
#include "Component.hpp"
#include "RigelHandle.hpp"
#include "Math.hpp"

namespace rge
{
    class Transform final : public Component
    {
    public:
        DEFINE_COMPONENT_TYPE(Transform);

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetScale(const glm::vec3& scale);

        NODISCARD glm::vec3 GetPosition() const { return m_Position; }
        NODISCARD glm::quat GetRotation() const { return m_Rotation; }
        NODISCARD glm::vec3 GetScale() const { return m_Scale; }

        NODISCARD nlohmann::json Serialize() const override;
        void Deserialize(const nlohmann::json& json) override;
    private:
        Transform();
        Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

        void Update();

        glm::vec3 m_Position;
        glm::quat m_Rotation;
        glm::vec3 m_Scale;

        bool m_UpdateRequiredFlag = true;

        static bool m_Registered;
        friend class GameObject;
    };
}
