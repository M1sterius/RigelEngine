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

    GOHandle Scene::InstantiateGO(std::string name)
    {
        const auto go = new GameObject(GetNextObjectID(), std::move(name));
        go->m_SceneID = this->GetID();
        m_Objects.push_back(go);

        if (IsLoaded())
        {
            go->OnLoad();
            go->OnStart();
        }

        return { go, go->GetID(), this->GetID() };
    }

    void Scene::DestroyGO(const GOHandle& handle)
    {
        for (size_t i = 0; i < m_Objects.size(); i++)
        {
            if (const auto go = m_Objects[i]; go->GetID() == handle.GetID())
            {
                if (IsLoaded())
                    go->OnDestroy();

                delete go;
                m_Objects.erase(m_Objects.begin() + i);

                return;
            }
        }

        Debug::Error("Failed to destroy game object with ID: " +
                std::to_string(handle.GetID()) + ". " +
                "Game object isn't present on the scene with ID: " + std::to_string(this->GetID()));
    }

    void Scene::OnLoad()
    {
        m_IsLoaded = true;
    }

    void Scene::OnUnload()
    {
        m_IsLoaded = false;
    }

    bool Scene::IsGOHandleValid(const GOHandle& handle) const
    {
        for (const auto obj : m_Objects)
            if (obj->GetID() == handle.GetID()) return true;
        return false;
    }

    nlohmann::json Scene::Serialize() const
    {
        auto json = nlohmann::json();
        json["ID"] = GetID();

        for (const auto& gameObject : m_Objects)
            json["GameObjects"].push_back(gameObject->Serialize());

        return json;
    }

    bool Scene::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("GameObjects") || !json.contains("ID"))
        {
            Debug::Error("Failed to deserialize rge::Scene! Some of the required data is not present in the json object.");
            return false;
        }

        if (!m_Objects.empty())
            Debug::Error("Attempted to deserialized a scene that is not empty! All objects already present will be overridden!");
        m_Objects.clear(); // Potential MEMORY LEAK!!!

        // Maybe there should be a rule that deserialization can only
        // be performed on an empty scene

        for (const auto& goJson : json["GameObjects"])
        {
            auto go = InstantiateGO();
            go->Deserialize(goJson);

            // TODO: Figure out the way to deal with IDs
        }

        return true;
    }

}
