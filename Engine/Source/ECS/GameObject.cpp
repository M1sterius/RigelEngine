#include "GameObject.hpp"

#include "Debug.hpp"
#include "ComponentTypeRegistry.hpp"
#include "Engine.hpp"
#include "SceneManager.hpp"
#include "json.hpp"

namespace rge
{
    GameObject::GameObject(std::string name) : RigelObject()
    {
        m_Name = std::move(name);
    }

    GameObject::~GameObject() = default;

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
