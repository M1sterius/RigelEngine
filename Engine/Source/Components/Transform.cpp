#include "Components/Transform.hpp"
#include "GLM_Serializer.hpp"
#include "Engine.hpp"
#include "EventManager.hpp"
#include "InternalEvents.hpp"
#include "Debug.hpp"
#include "json.hpp"

namespace Rigel
{
    Transform::Transform() : Component(),
        m_Position(glm::vec3(0.0f)),
        m_Rotation(glm::identity<glm::quat>()),
        m_Scale(glm::vec3(1.0f)) { }

    Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) : Component(),
        m_Position(position),
        m_Rotation(rotation),
        m_Scale(scale) { }

    void Transform::OnStart()
    {
        m_UpdateCallbackID = EventManager.Subscribe<Backend::TransformUpdateEvent>(
        [this](const Backend::TransformUpdateEvent&) { this->Update(); }
        );
    }

    void Transform::OnDestroy()
    {
        EventManager.Unsubscribe<Backend::TransformUpdateEvent>(m_UpdateCallbackID);
    }

    glm::vec3 Transform::GetPosition()
    {
        Update();
        return m_Position;
    }

    glm::quat Transform::GetRotation()
    {
        Update();
        return m_Rotation;
    }

    glm::vec3 Transform::GetScale()
    {
        Update();
        return m_Scale;
    }

    void Transform::SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        m_UpdateRequiredFlag = true;
    }

    void Transform::SetRotation(const glm::quat& rotation)
    {
        m_Rotation = rotation;
        m_UpdateRequiredFlag = true;
    }

    void Transform::SetScale(const glm::vec3& scale)
    {
        m_Scale = scale;
        m_UpdateRequiredFlag = true;
    }

    glm::mat4 Transform::GetModel()
    {
        Update();
        return m_ModelMatrix;
    }

    void Transform::Update()
    {
        if (!m_UpdateRequiredFlag) return;

        m_ModelMatrix = glm::translate(glm::mat4(1.0f), m_Position);
        m_ModelMatrix *= glm::mat4_cast(m_Rotation);
        m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);

        m_NormalMatrix = glm::mat3(glm::transpose(glm::inverse(m_ModelMatrix)));

        constexpr glm::vec4 UP = {0.0, 1.0, 0.0, 0.0};
        constexpr glm::vec4 FORWARD = {0.0, 0.0, -1.0, 0.0};
        constexpr glm::vec4 RIGHT = {1.0, 0.0, 0.0, 0.0};

        m_UpVector = glm::vec3(m_NormalMatrix * UP);
        m_ForwardVector = glm::vec3(m_NormalMatrix * FORWARD);
        m_RightVector = glm::vec3(m_NormalMatrix * RIGHT);

        m_UpdateRequiredFlag = false;
    }

    nlohmann::json Transform::Serialize() const
    {
        auto json = Component::Serialize();

        json["Position"] = GLM_Serializer::Serialize(m_Position);
        json["Rotation"] = GLM_Serializer::Serialize(m_Rotation);
        json["Scale"] = GLM_Serializer::Serialize(m_Scale);

        return json;
    }

    bool Transform::Deserialize(const nlohmann::json& json)
    {
        Component::Deserialize(json);

        if (json["Type"] != GetTypeName())
        {
            Debug::Error("Failed to deserialize Rigel::Transform component. Type mismatch!");
            return false;
        }

        if (!json.contains("Position") || ! json.contains("Rotation") || !json.contains("Scale"))
        {
            Debug::Error("Failed to deserialize Rigel::Transform. Some of the required data is missing!");
            return false;
        }

        m_Position = GLM_Serializer::DeserializeVec3(json["Position"]);
        m_Rotation = GLM_Serializer::DeserializeQuaternion(json["Rotation"]);
        m_Scale = GLM_Serializer::DeserializeVec3(json["Scale"]);

        return true;
    }
}
