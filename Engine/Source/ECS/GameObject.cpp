#include "ECS/GameObject.hpp"
#include "Debug.hpp"
#include "ECS/Scene.hpp"
#include "Components/Transform.hpp"
#include "Utilities/Reflection/TypeUtility.hpp"

#include "nlohmann_json/json.hpp"

#include <ranges>

namespace Rigel
{
    using namespace Backend::HandleValidation;

    GameObject::GameObject(const uid_t id, std::string name)
        : RigelObject(id), m_Name(std::move(name)) { }
    GameObject::~GameObject() = default;

    void GameObject::SetActive(const bool active)
    {
        if (m_Active == active) return;

        for (const auto& component : m_Components | std::views::values)
            component->SetActive(active);

        m_Active = active;
    }

    void GameObject::OnLoad()
    {
        for (const auto& component : m_Components | std::views::values)
            component->CallOnLoad();

        m_Loaded = true;
    }

    void GameObject::OnStart()
    {
        for (const auto& component : m_Components | std::views::values)
            component->CallOnStart();

        // This is used to preserve active state after deserialization
        if (!m_Active)
        {
            for (const auto& component : m_Components | std::views::values)
                component->SetActive(false);
        }
    }

    void GameObject::OnDestroy()
    {
        for (const auto& component : m_Components | std::views::values)
            component->CallOnDestroy();

        m_Loaded = false;
    }

    uid_t GameObject::AssignIDToComponent(Component* ptr)
    {
        const auto id = m_Scene->GetNextObjectID();
        ptr->OverrideID(id);
        return id;
    }

    nlohmann::json GameObject::Serialize() const
    {
        auto json = nlohmann::json();

        json["ID"] = GetID();
        json["Name"] = GetName();
        json["Active"] = m_Active;

        for (const auto& component : m_Components | std::views::values)
            json["Components"].push_back(component->Serialize());

        return json;
    }

    bool GameObject::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("Components") || !json.contains("ID") || !json.contains("Name") ||
            !json.contains("Active"))
        {
            Debug::Error("Failed to deserialize Rigel::GameObject! Some of the required data is not present in the json object.");
            return false;
        }

        m_Name = json["Name"].get<std::string>();
        OverrideID(json["ID"].get<uid_t>());
        m_Active = json["Active"].get<bool>();

        for (const auto& componentJson : json["Components"])
        {
            const auto typeString = componentJson["Type"].get<std::string>();
            auto typePtr = TypeRegistry::FindType(typeString);

            if (auto component = std::unique_ptr<Component>(dynamic_cast<Component*>(typePtr.release())))
            {
                component->m_Scene = m_Scene;
                component->m_GameObject = GOHandle(this, this->GetID());

                if (!component->Deserialize(componentJson))
                    continue; // if deserialization failed, std::unique_ptr will automatically delete the component

                // This line acquires type_index of DERIVED class type from a BASE class instance
                const auto derivedTypeIndex = TYPE_INDEX(*component);

                HandleValidator::AddHandle<HandleType::ComponentHandle>(component->GetID());
                m_Components[derivedTypeIndex] = std::move(component);
            }
            else
            {
                Debug::Error("Failed to serialize component of type: {}. "
                             "ComponentTypeRegistry returned nullptr!", typeString);
            }
        }

        return true;
    }
}
