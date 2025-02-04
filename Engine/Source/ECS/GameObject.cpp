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

    GameObject::~GameObject()
    {

    }

    nlohmann::json GameObject::Serialize() const
    {
        return nlohmann::json();
    }

    void GameObject::Deserialize(const nlohmann::json& json)
    {

    }
}
