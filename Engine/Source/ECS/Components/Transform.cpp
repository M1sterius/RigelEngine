#include "Components/Transform.hpp"
#include "GLM_Serializer.hpp"
#include "json.hpp"
#include "Debug.hpp"

#include <iostream>

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

    }

    nlohmann::json Transform::Serialize() const
    {
        auto json = nlohmann::json();
        json["ID"] = GetID();
        json["position"] = GLM_Serializer::Serialize(m_Position);
        json["rotation"] = GLM_Serializer::Serialize(m_Rotation);
        json["scale"] = GLM_Serializer::Serialize(m_Scale);
        return json;
    }

    void Transform::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("position") || !json.contains("rotation") || !json.contains("scale"))
        {
            Debug::Error("Failed to serialize rge::Transform component!");
            return;
        }

        SetPosition(GLM_Serializer::DeserializeVec3(json["position"]));
        SetRotation(GLM_Serializer::DeserializeQuaternion(json["rotation"]));
        SetScale(GLM_Serializer::DeserializeVec3(json["scale"]));
    }
}