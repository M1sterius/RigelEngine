#include "Scene.hpp"
#include "Engine.hpp"
#include "SceneManager.hpp"
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

    void Scene::OnLoad()
    {

    }

    void Scene::OnUnload()
    {

    }


    // Handle method definitions
    SceneHandle::SceneHandle(Scene* ptr, const uid_t id) : RigelHandle(ptr, id) { }

    bool SceneHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ObjectID == NULL_ID;
    }

    bool SceneHandle::IsValid() const
    {
        const auto& manager = Engine::Get().GetSceneManager();
        return manager.IsSceneHandleValid(*this);
    }
}