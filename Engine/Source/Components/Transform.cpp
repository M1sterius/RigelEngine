#include "Transform.hpp"
#include "Serializer.hpp"
#include "InternalEvents.hpp"
#include "Debug.hpp"
#include "Scene.hpp"

#include "json.hpp"

#include <ranges>

namespace Rigel
{
    Transform::Transform() : Component(),
         m_LocalPosition(glm::vec3(0.0f)),
         m_LocalRotation(glm::identity<glm::quat>()),
         m_LocalScale(glm::vec3(1.0f)),
         m_WorldPosition(m_LocalPosition),
         m_WorldRotation(m_LocalRotation),
         m_WorldScale(m_LocalScale) { }

    Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) : Component(),
        m_LocalPosition(position),
        m_LocalRotation(rotation),
        m_LocalScale(scale),
        m_WorldPosition(m_LocalPosition),
        m_WorldRotation(m_LocalRotation),
        m_WorldScale(m_LocalScale) { }

    void Transform::OnLoad()
    {
        SubscribeEvent<Backend::TransformUpdateEvent>(UpdateOnDemand);

        const auto thisHandle = ComponentHandle(this, this->GetID());

        // Convert deserialized children IDs into handles
        for (auto& child : m_Children)
        {
            const auto childGenericHandle = GetScene()->FindComponentByID(child.GetID());

            if (childGenericHandle.IsNull())
            {
                Debug::Error("A child transform component with ID {} not found!", child.GetID());
                continue;
            }

            child = childGenericHandle.Cast<Transform>();
            child->m_Parent = thisHandle;
        }
    }

    void Transform::SetLocalPosition(const glm::vec3& position)
    {
        m_LocalPosition = position;
        m_UpdateRequiredFlag = true;
    }

    void Transform::SetLocalRotation(const glm::quat& rotation)
    {
        m_LocalRotation = rotation;
        m_UpdateRequiredFlag = true;
    }

    void Transform::SetLocalScale(const glm::vec3& scale)
    {
        m_LocalScale = scale;
        m_UpdateRequiredFlag = true;
    }

    glm::vec3 Transform::GetPosition()
    {
        UpdateOnDemand();
        return m_WorldPosition;
    }

    glm::quat Transform::GetRotation()
    {
        UpdateOnDemand();
        return m_WorldRotation;
    }

    glm::vec3 Transform::GetScale()
    {
        UpdateOnDemand();
        return m_WorldScale;
    }

    glm::vec3 Transform::GetLocalPosition()
    {
        UpdateOnDemand();
        return m_LocalPosition;
    }

    glm::quat Transform::GetLocalRotation()
    {
        UpdateOnDemand();
        return m_LocalRotation;
    }

    glm::vec3 Transform::GetLocalScale()
    {
        UpdateOnDemand();
        return m_LocalScale;
    }

    glm::vec3 Transform::GetForwardVector()
    {
        UpdateOnDemand();
        return m_ForwardVector;
    }

    glm::vec3 Transform::GetRightVector()
    {
        UpdateOnDemand();
        return m_RightVector;
    }

    glm::vec3 Transform::GetUpVector()
    {
        UpdateOnDemand();
        return m_UpVector;
    }

    glm::mat4 Transform::GetLocalMatrix()
    {
        UpdateOnDemand();
        return m_LocalMatrix;
    }

    glm::mat4 Transform::GetWorldMatrix() // NOLINT(*-no-recursion)
    {
        UpdateOnDemand();
        return m_WorldMatrix;
    }

    void Transform::UpdateOnDemand() // NOLINT(*-no-recursion)
    {
        if (m_UpdateRequiredFlag)
        {
            UpdateImpl();
            m_UpdateRequiredFlag = false;
        }
    }

    void Transform::UpdateImpl() // NOLINT(*-no-recursion)
    {
        m_LocalMatrix = glm::translate(glm::mat4(1.0f), m_LocalPosition);
        m_LocalMatrix *= glm::mat4_cast(m_LocalRotation);
        m_LocalMatrix = glm::scale(m_LocalMatrix, m_LocalScale);

        // this will force all transforms up the hierarchy to update, so that we can get an up-to-date world matrix
        m_WorldMatrix = !m_Parent.IsNull() ? m_Parent->GetWorldMatrix() * m_LocalMatrix : m_LocalMatrix;

        m_WorldPosition = glm::vec3(m_WorldMatrix[3]);
        m_WorldRotation = glm::quat_cast(m_WorldMatrix);
        m_WorldScale = ExtractWorldScale(m_WorldMatrix);

        m_NormalMatrix = glm::mat3(glm::transpose(glm::inverse(m_WorldMatrix)));

        m_UpVector = glm::vec3(m_NormalMatrix * glm::vec4(WORLD_UP, 0.0f));
        m_ForwardVector = glm::vec3(m_NormalMatrix * glm::vec4(WORLD_FORWARD, 0.0f));
        m_RightVector = glm::vec3(m_NormalMatrix * glm::vec4(WORLD_RIGHT, 0.0f));

        // if parent component was updated, all it's children must be updated as well
        for (auto& child : m_Children)
            child->m_UpdateRequiredFlag = true;
    }

    glm::vec3 Transform::ExtractWorldScale(const glm::mat4& matrix)
    {
        glm::vec3 scale{};
        scale.x = glm::length(glm::vec3(matrix[0]));
        scale.y = glm::length(glm::vec3(matrix[1]));
        scale.z = glm::length(glm::vec3(matrix[2]));
        return scale;
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

        json["Position"] = Serializer::Serialize(m_LocalPosition);
        json["Rotation"] = Serializer::Serialize(m_LocalRotation);
        json["Scale"] = Serializer::Serialize(m_LocalScale);

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

        m_LocalPosition = Serializer::DeserializeVec3(json["Position"]);
        m_LocalRotation = Serializer::DeserializeQuaternion(json["Rotation"]);
        m_LocalScale = Serializer::DeserializeVec3(json["Scale"]);

        // write IDs only because not all objects on the scene are fully deserialized,
        // meaning we can't acquire actual handles yet
        for (const auto& child : json["Children"])
            m_Children.emplace_back(nullptr, child.get<uid_t>());

        return true;
    }
}
