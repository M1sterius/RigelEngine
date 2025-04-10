#pragma once

#include "Core.hpp"
#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"
#include "Math.hpp"

#include <iostream>

namespace Rigel
{
    class Transform final : public Component
    {
    public:
        RIGEL_REGISTER_COMPONENT(Transform);

        NODISCARD glm::vec3 GetPosition();
        NODISCARD glm::quat GetRotation();
        NODISCARD glm::vec3 GetScale();

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetScale(const glm::vec3& scale);

        NODISCARD glm::mat4 GetModel();

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

        glm::mat4 m_ModelMatrix{};
        glm::mat4 m_NormalMatrix{};
        glm::vec3 m_ForwardVector{};
        glm::vec3 m_RightVector{};
        glm::vec3 m_UpVector{};

        bool m_UpdateRequiredFlag = true;
        uid_t m_UpdateCallbackID = NULL_ID;
    };
}
