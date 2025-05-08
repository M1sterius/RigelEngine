#pragma once

#include "Core.hpp"
#include "Math.hpp"
#include "Component.hpp"
#include "ComponentHandle.hpp"

#include <vector>

namespace Rigel
{
    class Transform final : public Component
    {
    public:
        RIGEL_REGISTER_COMPONENT(Rigel::Transform);

        NODISCARD glm::vec3 GetPosition();
        NODISCARD glm::quat GetRotation();
        NODISCARD glm::vec3 GetScale();

        NODISCARD glm::vec3 GetLocalPosition();
        NODISCARD glm::quat GetLocalRotation();
        NODISCARD glm::vec3 GetLocalScale();

        void SetLocalPosition(const glm::vec3& position);
        void SetLocalRotation(const glm::quat& rotation);
        void SetLocalScale(const glm::vec3& scale);

        NODISCARD glm::mat4 GetLocalMatrix();
        NODISCARD glm::mat4 GetWorldMatrix();

        NODISCARD ComponentHandle<Transform> GetParent() const { return m_Parent; }

        void SetParent(ComponentHandle<Transform>& parent);
        void AddChild(ComponentHandle<Transform>& child);
        void RemoveChild(ComponentHandle<Transform>& child);

        NODISCARD const std::vector<ComponentHandle<Transform>>& GetChildren() const { return m_Children; }

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;
    private:
        Transform();
        Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

        void OnLoad() override;
        void UpdateOnDemand();

        void UpdateImpl();
        NODISCARD static glm::vec3 ExtractWorldScale(const glm::mat4& matrix);

        ComponentHandle<Transform> m_Parent{};
        std::vector<ComponentHandle<Transform>> m_Children{};

        glm::vec3 m_LocalPosition;
        glm::quat m_LocalRotation;
        glm::vec3 m_LocalScale;

        glm::vec3 m_WorldPosition;
        glm::quat m_WorldRotation;
        glm::vec3 m_WorldScale;

        glm::mat4 m_LocalMatrix{1.0};
        glm::mat4 m_WorldMatrix{1.0};
        glm::mat4 m_NormalMatrix{1.0};

        glm::vec3 m_ForwardVector{};
        glm::vec3 m_RightVector{};
        glm::vec3 m_UpVector{};

        bool m_UpdateRequiredFlag = true;
    };
}
