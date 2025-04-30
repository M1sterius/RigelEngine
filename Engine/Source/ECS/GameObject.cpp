#include "GameObject.hpp"
#include "Debug.hpp"
#include "Scene.hpp"
#include "Transform.hpp"
#include "json.hpp"
#include "ComponentTypeRegistry.hpp"

namespace Rigel
{
    using namespace Backend::HandleValidation;

    GameObject::GameObject(const uid_t id, std::string name)
        : RigelObject(id), m_Name(std::move(name)) { }
    GameObject::~GameObject() = default;

    uid_t GameObject::AssignIDToComponent(Component* ptr)
    {
        const auto id = m_Scene->GetNextObjectID();
        ptr->OverrideID(id);
        return id;
    }

    void GameObject::OnLoad()
    {
        m_Loaded = true;

        for (const auto& component : m_Components | std::views::values)
            component->CallOnLoad();
    }

    void GameObject::OnStart()
    {
        for (const auto& component : m_Components | std::views::values)
            component->CallOnStart();
    }

    void GameObject::OnDestroy()
    {
        for (const auto& component : m_Components | std::views::values)
            component->CallOnDestroy();

        m_Loaded = false;
    }

    nlohmann::json GameObject::Serialize() const
    {
        auto json = nlohmann::json();

        json["ID"] = GetID();
        json["Name"] = GetName();

        for (const auto& component : m_Components | std::views::values)
            json["Components"].push_back(component->Serialize());

        return json;
    }

    bool GameObject::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("Components") || !json.contains("ID") || !json.contains("Name"))
        {
            Debug::Error("Failed to deserialize Rigel::GameObject! Some of the required data is not present in the json object.");
            return false;
        }

        m_Name = json["Name"].get<std::string>();
        this->OverrideID(json["ID"].get<uid_t>());

        for (const auto& componentJson : json["Components"])
        {
            const auto typeString = componentJson["Type"].get<std::string>();

            if (auto component = ComponentTypeRegistry::FindType(typeString))
            {
                if (!component->Deserialize(componentJson))
                    continue; // if deserialization failed, std::unique_ptr will automatically delete the component

                component->m_Scene = m_Scene;
                component->m_GameObject = GOHandle(this, this->GetID());

                // This weird line acquires type_index of derived class type from a base class instance
                const auto derivedTypeIndex = std::type_index(typeid(*component));

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

    void GameObject::SetActive(const bool active)
    {
        if (m_Active == active) return;


    }
}
