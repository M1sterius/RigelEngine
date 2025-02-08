#include "GameObject.hpp"
#include "Debug.hpp"
#include "ComponentTypeRegistry.hpp"
#include "Engine.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "json.hpp"

namespace rge
{
    GameObject::GameObject(const uid_t id, std::string name) : RigelObject(id)
    {
        m_Name = std::move(name);
    }

    GameObject::~GameObject() = default;

    uid_t GameObject::AssignIDToComponent(Component* ptr) const
    {
        const auto& manager = Engine::Get().GetSceneManager();
        auto scene = manager.GetSceneByID(m_SceneID);
        const auto id = scene->GetNextObjectID();
        ptr->OverrideID(id);
        return id;
    }

    nlohmann::json GameObject::Serialize() const
    {
        auto json = nlohmann::json();

        json["ID"] = GetID();

        for (const auto& [_, component] : m_Components)
            json["Components"].push_back(component->Serialize());

        return json;
    }

    void GameObject::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("Components") || !json.contains("ID"))
        {
            Debug::Error("Failed to serialize rge::GameObject!");
            return;
        }

        OverrideID(json["ID"].get<uid_t>());

        /*
         * Iterate through all components inside the json array,
         * retrieve the type represented as a string and then look it up in the registry.
         * The registry will return a nullptr if the type is not found.
         */
        for (const auto& component : json["Components"])
        {
            const auto type = component["Type"].get<std::string>();

            if (const auto cmpPtr = ComponentTypeRegistry::FindType(type); cmpPtr != nullptr)
            {
                cmpPtr->Deserialize(component);
                m_Components[cmpPtr->GetID()] = cmpPtr;
            }
            else
                Debug::Error("Failed to serialize component of type: " + type);
        }
    }
}
