#include "Scene.hpp"
#include "Engine.hpp"
#include "EventManager.hpp"
#include "InternalEvents.hpp"
#include "HandleValidator.hpp"
#include "GameObject.hpp"
#include "Transform.hpp"
#include "GOHandle.hpp"
#include "json.hpp"

namespace Rigel
{
    using namespace Backend::HandleValidation;

    Scene::Scene(const uid_t id, std::string name)
        : RigelObject(id), m_Name(std::move(name))
    {
        HandleValidator::AddHandle<HandleType::SceneHandle>(this->GetID());
    }

    Scene::~Scene()
    {
        HandleValidator::RemoveHandle<HandleType::SceneHandle>(this->GetID());
    }

    GOHandle Scene::Instantiate(std::string name)
    {
        const auto go = new GameObject(GetNextObjectID(), std::move(name));
        go->m_Scene = SceneHandle(this, this->GetID());
        go->AddComponent<Transform>();

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

        return {go, go->GetID()};
    }

    void Scene::DestroyGOImpl(const uid_t id)
    {
        for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); ++it)
        {
            auto& currentObject = *it;

            if (currentObject->GetID() == id)
            {
                if (IsLoaded())
                    currentObject->OnDestroy();

                m_GameObjects.erase(it);
                return;
            }
        }

        Debug::Error("Failed to destroy game object with ID {}. "
                     "Game object isn't present on the scene with ID {}.", id, this->GetID());
    }

    void Scene::Destroy(const GOHandle& handle)
    {
        if (!m_IsLoaded)
            DestroyGOImpl(handle.GetID());
        else
            m_DestroyQueue.push(handle);
    }

    void Scene::OnLoad()
    {
        m_EndOfFrameCallbackID = Engine::Get().GetEventManager().Subscribe<Backend::EndOfFrameEvent>(
            [this](const Backend::EndOfFrameEvent&){
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
            DestroyGOImpl(go->GetID());

        m_IsLoaded = false;

        Engine::Get().GetEventManager().Unsubscribe<Backend::EndOfFrameEvent>(m_EndOfFrameCallbackID);
    }

    void Scene::OnEndOfFrame()
    {
        // GameObject destruction is deferred until the end of frame to optimize
        // resource and memory management

        while (!m_DestroyQueue.empty())
        {
            const auto currentHandle = m_DestroyQueue.front();
            DestroyGOImpl(currentHandle.GetID());
            m_DestroyQueue.pop();
        }
    }

    GOHandle Scene::GetGameObjectByID(const uid_t id) const
    {
        for (const auto& go : m_GameObjects)
        {
            if (go->GetID() == id)
                return {go.get(), id};
        }

        return GOHandle::Null();
    }

    plf::colony<GOHandle> Scene::Search(const std::function<bool(GOHandle&)>& condition, const size_t depthLimit) const
    {
        auto objects = plf::colony<GOHandle>();
        size_t depth = 0;

        for (const auto& go : m_GameObjects)
        {
            if (++depth > depthLimit)
                return objects;

            auto curHandle = GOHandle(go.get(), go->GetID());
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

        if (!json.contains("ID") || !json.contains("Name") || !json.contains("GameObjects"))
        {
            Debug::Error("Failed to deserialize Rigel::Scene! Some of the required data is not present in the json object.");
            return false;
        }

        if (!m_GameObjects.empty())
        {
            Debug::Error("Attempted to deserialized a scene that is not empty! Destroy all objects already instantiated and try again.");
            return false;
        }

        m_Name = json["Name"].get<std::string>();

        for (const auto& goJson : json["GameObjects"])
        {
            const auto go = new GameObject(GetNextObjectID(), "GameObject");
            go->m_Scene = SceneHandle(this, this->GetID());

            if (!go->Deserialize(goJson))
            {
                delete go;
                continue;
            }

            m_GameObjects.emplace(std::unique_ptr<GameObject>(go));
        }

        return true;
    }
}
