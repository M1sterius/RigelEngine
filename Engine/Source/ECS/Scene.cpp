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

    bool Scene::IsGOHandleValid(const GOHandle& handle) const
    {
        if (const auto it = m_Objects.find(handle.GetID()); it != m_Objects.end())
            return true;
        else return false;
    }


    // Handle method definitions
    SceneHandle::SceneHandle(Scene* ptr, const uid_t id) : RigelHandle(ptr, id) { }

    bool SceneHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ID == NULL_ID;
    }

    bool SceneHandle::IsValid() const
    {
        const auto& manager = Engine::Get().GetSceneManager();
        return manager.IsSceneHandleValid(*this);
    }
}
