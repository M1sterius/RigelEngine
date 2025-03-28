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

    GOHandle Scene::InstantiateGO(std::string name)
    {
        const auto go = new GameObject(GetNextObjectID(), std::move(name));
        go->m_Scene = SceneHandle(this, this->GetID());
        m_GameObjects.emplace_back(std::unique_ptr<GameObject>(go));

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
        for (size_t i = 0; i < m_GameObjects.size(); i++)
        {
            if (auto& go = m_GameObjects[i]; go->GetID() == handle.GetID())
            {
                if (IsLoaded())
                    go->OnDestroy();

                go.reset();
                m_GameObjects.erase(m_GameObjects.begin() + i);

                return;
            }
        }

        Debug::Error("Failed to destroy game object with ID {}. "
                     "Game object isn't present on the scene with ID {}.", handle.GetID(), this->GetID());
    }

    void Scene::OnLoad()
    {
        m_IsLoaded = true;

        for (const auto& go : m_GameObjects)
        {
            go->OnLoad();
            go->OnStart();
        }
    }

    void Scene::OnUnload()
    {
        for (const auto& go : m_GameObjects)
            go->OnDestroy();

        m_IsLoaded = false;
    }

    bool Scene::ValidateGOHandle(const GOHandle& handle) const
    {
        for (const auto& obj : m_GameObjects)
            if (obj->GetID() == handle.GetID()) return true;
        return false;
    }

    nlohmann::json Scene::Serialize() const
    {
        auto json = nlohmann::json();

        json["ID"] = GetID();
        json["Name"] = GetName();

        for (const auto& gameObject : m_GameObjects)
            json["GameObjects"].push_back(gameObject->Serialize());

        return json;
    }

    bool Scene::Deserialize(const nlohmann::json& json)
    {
        if (IsLoaded())
        {
            Debug::Error("Deserialization on a loaded scene is not allowed. Deserialization aborted!");
            return false;
        }

        if (!json.contains("GameObjects") || !json.contains("ID") || !json.contains("Name"))
        {
            Debug::Error("Failed to deserialize rge::Scene! Some of the required data is not present in the json object.");
            return false;
        }

        if (!m_GameObjects.empty())
        {
            Debug::Error("Attempted to deserialized a scene that is not empty! All objects already present will be deleted!");

            for (auto& go : m_GameObjects)
                go.reset();

            m_GameObjects.clear();
        }

        m_Name = json["Name"].get<std::string>();

        for (const auto& goJson : json["GameObjects"])
        {
            if (auto go = InstantiateGO(); !go->Deserialize(goJson))
                DestroyGO(go);
        }

        return true;
    }

    std::vector<GOHandle> Scene::Search(const std::function<bool(const GOHandle&)>& condition, const size_t countLimit) const
    {
        std::vector<GOHandle> objects;
        objects.reserve(std::min(countLimit, m_GameObjects.size()));

        for (const auto& go : m_GameObjects)
        {
            const auto handle = GOHandle(go.get(), go->GetID(), GetID());

            if (condition(handle))
            {
                objects.push_back(handle);

                if (objects.size() >= countLimit)
                    break;
            }
        }

        return objects;
    }
}
