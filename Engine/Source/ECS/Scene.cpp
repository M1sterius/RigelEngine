#include "Scene.hpp"
#include "GameObject.hpp"
#include "GOHandle.hpp"
#include "json.hpp"
#include "Transform.hpp"

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
        go->m_Scene = SceneHandle(this, this->GetID());
        go->AddComponent<Transform>(); // Every game object must have a transform component attached to it
        m_Objects.push_back(go);

        /*
         * If the scene is loaded, appropriate event functions must be invoked
         * to ensure proper initialization.
         *
         * Before the scene becomes loaded, all objects on it are NOT instantiated
         */
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

        for (const auto& go : m_Objects)
        {
            go->OnLoad();
            go->OnStart();
        }
    }

    void Scene::OnUnload()
    {
        for (const auto& go : m_Objects)
            go->OnDestroy();

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
        json["Name"] = GetName();

        for (const auto& gameObject : m_Objects)
            json["GameObjects"].push_back(gameObject->Serialize());

        return json;
    }

    bool Scene::Deserialize(const nlohmann::json& json)
    {
        if (IsLoaded())
        {
            Debug::Error("Attempted to deserialize a scene that is already loaded. That behaviour is currently unsupported. Deserialization aborted!");
            return false;
        }

        if (!json.contains("GameObjects") || !json.contains("ID") || !json.contains("Name"))
        {
            Debug::Error("Failed to deserialize rge::Scene! Some of the required data is not present in the json object.");
            return false;
        }

        if (!m_Objects.empty())
        {
            Debug::Error("Attempted to deserialized a scene that is not empty! All objects already present will be deleted!");

            for (const auto& go : m_Objects)
                delete go; // TODO: Fix a memory leak caused by not deleting components in the GameObject class
        }

        for (const auto& goJson : json["GameObjects"])
        {
            if (auto go = InstantiateGO(); !go->Deserialize(goJson))
                DestroyGO(go);
        }

        return true;
    }

}
