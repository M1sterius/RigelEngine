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
        : RigelObject(id), m_Name(std::move(name))
    {
        HandleValidator::AddHandle<HandleType::GOHandle>(this->GetID());
    }

    GameObject::~GameObject()
    {
        HandleValidator::RemoveHandle<HandleType::GOHandle>(this->GetID());
    }

    uid_t GameObject::AssignIDToComponent(Component* ptr)
    {
        const auto id = m_Scene->GetNextObjectID();
        ptr->OverrideID(id);
        return id;
    }

    void GameObject::OnLoad()
    {
        for (const auto& component : m_Components | std::views::values)
            component->OnLoad();
    }

    void GameObject::OnStart()
    {
        for (const auto& component : m_Components | std::views::values)
            component->OnStart();
    }

    void GameObject::OnDestroy()
    {
        for (const auto& component : m_Components | std::views::values)
            component->OnDestroy();
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
            Debug::Error("Failed to serialize Rigel::GameObject! Some of the required data is not present in the json object.");
            return false;
        }

        HandleValidator::RemoveHandle<HandleType::GOHandle>(this->GetID());

        m_Name = json["Name"].get<std::string>();
        OverrideID(json["ID"].get<uid_t>());

        // This is done so that the proper ID will be in the validation list after the deserialization
        HandleValidator::AddHandle<HandleType::GOHandle>(this->GetID());

        for (const auto& component : json["Components"])
        {
            const auto type = component["Type"].get<std::string>();

            if (const auto cmpPtr = ComponentTypeRegistry::FindType(type); cmpPtr != nullptr)
            {
                cmpPtr->m_Scene = m_Scene;
                cmpPtr->m_GameObject = GOHandle(this, this->GetID());
                cmpPtr->Deserialize(component);

                m_Components[cmpPtr->GetID()] = std::unique_ptr<Component>(cmpPtr);
            }
            else
                Debug::Error("Failed to serialize component of type: {}!", type);
        }

        return true;
    }
}
