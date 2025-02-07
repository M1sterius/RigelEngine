#include "Scene.hpp"
#include "GameObject.hpp"
#include "GOHandle.hpp"
#include "json.hpp"

namespace rge
{
    Scene::Scene(const uid_t id, std::string name) : RigelObject(id)
    {
        m_Name = std::move(name);
    }

    Scene::~Scene() = default;

    GOHandle Scene::AddGameObject(std::string name)
    {
        const auto go = new GameObject(m_NextObjectID++, std::move(name));
        const auto id = go->GetID();
        go->m_SceneID = this->GetID(); // assigning the ID of the scene that owns this game object

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
        return false;
    }

    nlohmann::json Scene::Serialize() const
    {
        auto json = nlohmann::json();
        json["ID"] = GetID();
        return json;
    }

    void Scene::Deserialize(const nlohmann::json& json)
    {

    }

}
