#include "Components/Transform.hpp"
#include "GLM_Serializer.hpp"
#include "Engine.hpp"
#include "EventManager.hpp"
#include "InternalEvents.hpp"
#include "Debug.hpp"
#include "json.hpp"

namespace rge
{
    Transform::Transform() : Component(),
        m_Position(glm::vec3(0.0f)), m_Rotation(glm::identity<glm::quat>()), m_Scale(glm::vec3(1.0f))
    {

    }

    Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) : Component(),
        m_Position(position), m_Rotation(rotation), m_Scale(scale)
    {

    }

    void Transform::OnStart()
    {
        m_UpdateCallbackID = EventManager.Subscribe<backend::TransformUpdateEvent>(
        [this](const backend::TransformUpdateEvent&) { this->Update(); }
        );
    }

    void Transform::OnDestroy()
    {
        EventManager.Unsubscribe<backend::TransformUpdateEvent>(m_UpdateCallbackID);
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

    void Transform::Update()
    {
        if (!m_UpdateRequiredFlag) return;

        m_ModelMatrix = glm::translate(m_ModelMatrix, m_Position);
        m_ModelMatrix *= glm::mat4_cast(m_Rotation);
        m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);

        m_NormalMatrix = glm::mat3(glm::transpose(glm::inverse(m_ModelMatrix)));

        m_UpdateRequiredFlag = false;
    }

    nlohmann::json Transform::Serialize() const
    {
        auto json = Component::Serialize(); // Base class method must be explicitly called
        json["position"] = GLM_Serializer::Serialize(m_Position);
        json["rotation"] = GLM_Serializer::Serialize(m_Rotation);
        json["scale"] = GLM_Serializer::Serialize(m_Scale);
        return json;
    }

    bool Transform::Deserialize(const nlohmann::json& json)
    {
        Component::Deserialize(json);

        if (json["Type"] != GetTypeName())
        {
            Debug::Error("Failed to deserialize rge::Transform component!");
            return false;
        }

        SetPosition(GLM_Serializer::DeserializeVec3(json["position"]));
        SetRotation(GLM_Serializer::DeserializeQuaternion(json["rotation"]));
        SetScale(GLM_Serializer::DeserializeVec3(json["scale"]));

        return true;
    }
}
