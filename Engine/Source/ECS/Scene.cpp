#include "Scene.hpp"
#include "Engine.hpp"
#include "SceneManager.hpp"
#include "GameObject.hpp"
#include "UIDGenerator.hpp"

namespace rge
{
    Scene::Scene(std::string name)
    {
        m_ID = UIDGenerator::Generate();
        m_Name = std::move(name);
    }

    Scene::~Scene()
    {

    }

    GOHandle Scene::AddGameObject(std::string name)
    {
        const auto go = new GameObject(std::move(name));
        const auto id = go->GetID();
        go->m_SceneID = m_ID; // assigning the ID of the scene that owns this game object

        m_Objects[id] = go;
        return {go, id, go->GetSceneID()};
    }

    void Scene::OnLoad()
    {

    }

    void Scene::OnUnload()
    {

    }

    bool Scene::IsGOHandleValid(const GOHandle& handle) const
    {
        if (const auto it = m_Objects.find(handle.GetID()); it != m_Objects.end())
            return true;
        else return false;
    }


    // Handle methods definitions

}
