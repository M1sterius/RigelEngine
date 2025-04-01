#include "Scene.hpp"
#include "Engine.hpp"
#include "EventManager.hpp"
#include "InternalEvents.hpp"
#include "GameObject.hpp"
#include "GOHandle.hpp"
#include "json.hpp"

namespace rge
{
    Scene::Scene(const uid_t id, std::string name) : RigelObject(id)
    {
        m_Name = std::move(name);
    }

    GOHandle Scene::Instantiate(std::string name)
    {
        const auto go = new GameObject(GetNextObjectID(), std::move(name));
        go->m_Scene = SceneHandle(this, this->GetID());
        m_GameObjects.emplace(std::unique_ptr<GameObject>(go));

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

    void Scene::Destroy(const GOHandle& handle)
    {
        if (!m_IsLoaded)
            DestroyGOImpl(handle);
        else
            m_DestroyQueue.push(handle);
    }

    void Scene::OnLoad()
    {
        m_EndOfFrameCallbackID = Engine::Get().GetEventManager().Subscribe<backend::EndOfFrameEvent>(
            [this](const backend::EndOfFrameEvent&){
            OnEndOfFrame();
        });

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

        Engine::Get().GetEventManager().Unsubscribe<backend::EndOfFrameEvent>(m_EndOfFrameCallbackID);
    }

    void Scene::OnEndOfFrame()
    {
        // GameObject destruction is deferred until the end of frame to optimize
        // resource and memory management

        while (!m_DestroyQueue.empty())
        {
            const auto currentHandle = m_DestroyQueue.front();
            DestroyGOImpl(currentHandle);
            m_DestroyQueue.pop();
        }
    }

    void Scene::DestroyGOImpl(const GOHandle& handle)
    {
        for (auto it = m_GameObjects.begin(); it != m_GameObjects.end();)
        {
            if ((*it)->GetID() == handle.GetID())
            {
                if (IsLoaded())
                    (*it)->OnDestroy();

                m_GameObjects.erase(it);
                return;
            }

            ++it; // move to the next element
        }

        Debug::Error("Failed to destroy game object with ID {}. "
                     "Game object isn't present on the scene with ID {}.", handle.GetID(), this->GetID());
    }

    bool Scene::ValidateGOHandle(const GOHandle& handle) const
    {
        for (const auto& obj : m_GameObjects)
            if (obj->GetID() == handle.GetID()) return true;
        return false;
    }

    GOHandle Scene::GetGameObjectByID(const uid_t id) const
    {
        for (const auto& go : m_GameObjects)
        {
            if (go->GetID() == id)
                return GOHandle(go.get(), id, this->GetID());
        }

        return GOHandle(nullptr, NULL_ID, NULL_ID);
    }

    plf::colony<GOHandle> Scene::Search(const std::function<bool(GOHandle&)>& condition, const size_t depthLimit) const
    {
        auto objects = plf::colony<GOHandle>();
        size_t depth = 0;

        for (const auto& go : m_GameObjects)
        {
            if (++depth > depthLimit)
                return objects;

            auto curHandle = GOHandle(go.get(), go->GetID(), this->GetID());
            if (condition(curHandle))
                objects.insert(curHandle);
        }

        return objects;
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
            if (auto go = Instantiate(); !go->Deserialize(goJson))
                Destroy(go);
        }

        return true;
    }
}
