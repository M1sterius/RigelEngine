#include "GameObject.hpp"
#include "UIDGenerator.hpp"
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

    }
}
