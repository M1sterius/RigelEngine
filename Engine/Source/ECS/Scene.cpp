#include "ECS/Scene.hpp"
#include "ECS/GameObject.hpp"
#include "InternalEvents.hpp"
#include "Components/Transform.hpp"
#include "Handles/HandleValidator.hpp"
#include "Handles/GOHandle.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/EventSystem/EventManager.hpp"

#include "json.hpp"

namespace Rigel
{
    using namespace Backend::HandleValidation;

    Scene::Scene(const uid_t id, std::string name)
        : RigelObject(id), m_Name(std::move(name)) { }
    Scene::~Scene() = default;

    GOHandle Scene::Instantiate(std::string name)
    {
        const auto go = new GameObject(GetNextObjectID(), std::move(name));
        go->m_Scene = SceneHandle(this, this->GetID());
        go->AddComponent<Transform>();

        HandleValidator::AddHandle<HandleType::GOHandle>(go->GetID());
        m_GameObjects.emplace(std::unique_ptr<GameObject>(go));

        /*
         * If the scene is loaded, appropriate event functions must be invoked
         * to ensure proper initialization.
         *
         * Before the scene becomes loaded, all objects on it are NOT instantiated
         */
        if (m_Loaded)
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
                if (m_Loaded)
                    currentObject->OnDestroy();

                HandleValidator::AddHandle<HandleType::GOHandle>(currentObject->GetID());
                m_GameObjects.erase(it);
                return;
            }
        }

        Debug::Error("Failed to destroy game object with ID {}. "
                     "Game object isn't present on the scene with ID {}.", id, this->GetID());
    }

    void Scene::Destroy(const GOHandle& handle)
    {
        if (!m_Loaded)
            DestroyGOImpl(handle.GetID());
        else
            m_DestroyQueue.push(handle);
    }

    void Scene::OnLoad()
    {
        m_Loaded = true;

        m_EndOfFrameCallbackID = GetEventManager()->Subscribe<Backend::EndOfFrameEvent>(
            [this](const Backend::EndOfFrameEvent&){
            OnEndOfFrame();
        });

        // Note that OnStart is called after ALL OnLoad invocations for all GOs,
        // this is extremely critical for proper resource management
        for (const auto& go : m_GameObjects)
            go->OnLoad();
        for (const auto& go : m_GameObjects)
            go->OnStart();
    }

    void Scene::OnUnload()
    {
        for (const auto& go : m_GameObjects)
            DestroyGOImpl(go->GetID());

        GetEventManager()->Unsubscribe<Backend::EndOfFrameEvent>(m_EndOfFrameCallbackID);

        m_Loaded = false;
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

    GOHandle Scene::FindGameObjectByID(const uid_t id) const
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
        json["ExtensionPath"] = ""; // This will be implemented later
        json["Version"] = 1u;

        // this makes sure that there is no ID overlap between serialized object and newly created ones
        json["NextObjectID"] = m_NextObjectID;

        for (const auto& gameObject : m_GameObjects)
            json["GameObjects"].push_back(gameObject->Serialize());

        return json;
    }

    bool Scene::Deserialize(const nlohmann::json& json)
    {
        if (m_Loaded)
        {
            Debug::Error("Deserialization on a loaded scene is not allowed. Deserialization aborted!");
            return false;
        }

        if (!json.contains("ID") || !json.contains("Name") || !json.contains("GameObjects") ||
            !json.contains("NextObjectID") || !json.contains("Version"))
        {
            Debug::Error("Failed to deserialize Rigel::Scene! Some of the required data is not present in the json object.");
            return false;
        }

        if (!m_GameObjects.empty())
        {
            Debug::Error("Attempted to deserialized a scene that is not empty! Destroy all objects already instantiated and try again.");
            return false;
        }

        // Currently only one version of scene json exists
        if (const auto version = json["Version"].get<uint32_t>(); version != 1u)
        {
            Debug::Error("Scene json version {} is not supported!", version);
            return false;
        }

        m_Name = json["Name"].get<std::string>();
        m_NextObjectID = json["NextObjectID"].get<uid_t>();

        for (const auto& goJson : json["GameObjects"])
        {
            // Pass empty name and NULL_ID because they will be overridden during deserialization anyway
            auto go = std::unique_ptr<GameObject>(new GameObject(NULL_ID, ""));

            go->m_Scene = SceneHandle(this, this->GetID());

            if (!go->Deserialize(goJson))
                continue; // if deserialization failed, std::unique_ptr will automatically delete the object

            HandleValidator::AddHandle<HandleType::GOHandle>(go->GetID());
            m_GameObjects.emplace(std::move(go));
        }

        return true;
    }
}
