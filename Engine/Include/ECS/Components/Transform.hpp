#pragma once

#include "HeaderUtils.hpp"
#include "Component.hpp"
#include "RigelHandle.hpp"
#include "glm.hpp"
#include "gtx/quaternion.hpp"

namespace rge
{
    class Transform final : public Component
    {
    public:
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

        friend class GameObject;
    };
}
