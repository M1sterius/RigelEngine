#include "Transform.hpp"
#include "GLM_Serializer.hpp"
#include "InternalEvents.hpp"
#include "Debug.hpp"

#include "json.hpp"

#include <ranges>

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
        SubscribeEvent<Backend::TransformUpdateEvent>(Update);
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

    void Transform::SetParent(ComponentHandle<Transform>& parent)
    {
        auto thisHandle = ComponentHandle(this, GetID());

        if (parent.GetID() == GetID())
        {
            Debug::Error("Cannot make a Transform a child of itself!");
            return;
        }

        if (!m_Parent.IsNull())
            m_Parent->RemoveChild(thisHandle);

        parent->AddChild(thisHandle);
    }

    void Transform::AddChild(ComponentHandle<Transform>& child)
    {
        if (child.GetID() == GetID())
        {
            Debug::Error("Cannot parent a Transform to itself!");
            return;
        }

        if (auto parent = child->GetParent(); !parent.IsNull())
            parent->RemoveChild(child);

        m_Children.push_back(child);
        child->m_Parent = ComponentHandle(this, GetID());
    }

    void Transform::RemoveChild(ComponentHandle<Transform>& child)
    {
        const auto it = std::ranges::find_if(m_Children, [child](const auto& curChild)
        {
            return child.GetID() == curChild.GetID();
        });

        if (it == m_Children.end())
        {
            Debug::Error("Child Transform with ID {} does not belong to Transform with ID {}!", child.GetID(), GetID());
            return;
        }

        (*it)->m_Parent = ComponentHandle<Transform>::Null();
        m_Children.erase(it);
    }

    nlohmann::json Transform::Serialize() const
    {
        auto json = Component::Serialize();

        json["Position"] = GLM_Serializer::Serialize(m_Position);
        json["Rotation"] = GLM_Serializer::Serialize(m_Rotation);
        json["Scale"] = GLM_Serializer::Serialize(m_Scale);

        json["Children"] = nlohmann::json::array(); // This insures that json always has 'Children' array field
        for (const auto& child : m_Children)
            json["Children"].push_back(child.GetID());

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

        if (!json.contains("Position") || ! json.contains("Rotation") || !json.contains("Scale") ||
            !json.contains("Children"))
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
