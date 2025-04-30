#include "Component.hpp"
#include "Debug.hpp"
#include "json.hpp"

#include <ranges>

namespace Rigel
{
    // The NULL_ID will be overwritten by GameObject::AddComponent method
    Component::Component() : RigelObject(NULL_ID) { }
    Component::~Component() = default;

    void Component::SetActive(const bool active)
    {
        if (m_Active == active) return;

        if (m_Active == true && active == false)
            CallOnDisable();
        else
            CallOnEnable();

        m_Active = active;
    }

    void Component::CallOnLoad()
    {
        OnLoad();
    }

    void Component::CallOnStart()
    {
        OnStart();
    }

    void Component::CallOnDestroy()
    {
        OnDestroy();

        auto& eventManager = Engine::Get().GetEventManager();

        for (const auto& [typeIndex, idCallbackPair] : m_EventsRegistry)
            eventManager.Unsubscribe(typeIndex, idCallbackPair.first);

        m_EventsRegistry.clear();
    }

    void Component::CallOnEnable()
    {
        OnEnable();

        auto& eventManager = Engine::Get().GetEventManager();

        // Resubscribe to events
        for (auto& [typeIndex, idCallbackPair] : m_EventsRegistry)
        {
            if (idCallbackPair.first == NULL_ID)
                eventManager.Subscribe(typeIndex, idCallbackPair.second);
        }
    }

    void Component::CallOnDisable()
    {
        OnDisable();

        auto& eventManager = Engine::Get().GetEventManager();

        for (auto& [typeIndex, idCallbackPair] : m_EventsRegistry)
        {
            eventManager.Unsubscribe(typeIndex, idCallbackPair.first);
            idCallbackPair.first = NULL_ID;
        }

        // Note that we don't clear the registry because we want to resubscribe to the same events in OnEnable
    }

    nlohmann::json Component::Serialize() const
    {
        auto json = nlohmann::json();

        json["Type"] = GetTypeName();
        json["ID"] = GetID();

        return json;
    }

    bool Component::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("ID"))
        {
            Debug::Error("Failed to deserialize Rigel::Component! Some of the required data is not present in the json object.");
            return false;
        }

        this->OverrideID(json["ID"].get<uid_t>());

        return true;
    }
}
