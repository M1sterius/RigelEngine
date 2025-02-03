#include "GameObject.hpp"
#include "UIDGenerator.hpp"
#include "Engine.hpp"
#include "SceneManager.hpp"

namespace rge
{

    GameObject::GameObject(std::string name)
    {
        m_ID = UIDGenerator::Generate();
        m_Name = std::move(name);
    }

    GameObject::~GameObject()
    {

    }
}
