#include <utility>

#include "ECS/Scene.hpp"
#include "SceneManager.hpp"
#include "GameObject.hpp"
#include "UIDGenerator.hpp"
#include "Engine.hpp"

namespace rge
{
    Scene::Scene(std::string name)
        : RigelObject(), m_Name(std::move(name))
    {
        m_ID = UIDGenerator::Generate();
    }
    Scene::~Scene() = default;

    void Scene::OnLoad()
    {
        rge::Logger::VerboseMessage("Loading scene: " + m_Name);

        for (auto& object : m_Objects)
            object.second->OnLoad();
    }

    void Scene::OnUnload()
    {
        rge::Logger::VerboseMessage("Unloading scene: " + m_Name);

        for (auto& object : m_Objects)
            object.second->OnUnload();
    }

    void Scene::OnGameUpdate()
    {
        for (auto& object : m_Objects)
            object.second->OnGameUpdate();
    }

    GOHandle Scene::AddGameObject()
    {
        auto go = new GameObject("New Game Object");

        m_Objects[go->GetID()] = go;

        return {go, go->GetID()};
    }

    bool Scene::CheckGOValidity(const uid_t id)
    {
        auto it = m_Objects.find(id);
        if (it != m_Objects.end())
            return true;
        return false;
    }

    bool Scene::IsLoaded() const
    {
        auto& manager = rge::Engine::Get()->GetSceneManager();
        return m_ID == manager.GetLoadedScene().GetID();
    }

    // Handle
    SceneHandle::SceneHandle(Scene* ptr, const uid_t id) : RigelHandle<Scene>(ptr, id) { }
    bool SceneHandle::IsValid() const
    {
        const auto& sceneManager = rge::Engine::Get()->GetSceneManager();
        return sceneManager.IsSceneHandleValid(*this);
    }
    bool SceneHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ObjectID == NULL_ID;
    }
}
