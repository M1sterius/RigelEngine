#include "ECS/Component.hpp"
#include "Debug.hpp"
#include "Subsystems/SubsystemGetters.hpp"

#include "nlohmann_json/json.hpp"

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
        m_Loaded = true;
    }

    void Component::CallOnStart()
    {
        OnStart();

        // This is used to preserve active state after deserialization,
        // note that the component will be disabled AFTER both OnLoad and OnStart ran
        if (!m_Active)
            CallOnDisable();
    }

    void Component::CallOnDestroy()
    {
        OnDestroy();

        for (const auto& [typeIndex, id] : m_EventsRegistry)
            GetEventManager()->Unsubscribe(typeIndex, id);
        m_EventsRegistry.clear();

        m_Loaded = false;
    }

    void Component::CallOnEnable()
    {
        OnEnable();

        for (const auto id : m_EventsRegistry | std::views::values)
            GetEventManager()->SetSuspend(id, false);
    }

    void Component::CallOnDisable()
    {
        OnDisable();

        for (const auto id : m_EventsRegistry | std::views::values)
            GetEventManager()->SetSuspend(id, true);
    }

    nlohmann::json Component::Serialize() const
    {
        auto json = nlohmann::json();

        json["Type"] = GetTypeName();
        json["ID"] = GetID();
        json["Active"] = m_Active;

        return json;
    }

    bool Component::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("ID") || !json.contains("Active"))
        {
            Debug::Error("Failed to deserialize Rigel::Component! Some of the required data is not present in the json object.");
            return false;
        }

        OverrideID(json["ID"].get<uid_t>());
        m_Active = json["Active"].get<bool>();

        return true;
    }
}
