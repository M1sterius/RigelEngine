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

        for (const auto& [typeIndex, id] : m_EventsRegistry)
            Engine::Get().GetEventManager().Unsubscribe(typeIndex, id);
        m_EventsRegistry.clear();
    }

    void Component::CallOnEnable()
    {
        OnEnable();

        for (const auto id : m_EventsRegistry | std::views::values)
            Engine::Get().GetEventManager().SetSuspend(id, false);
    }

    void Component::CallOnDisable()
    {
        OnDisable();

        for (const auto id : m_EventsRegistry | std::views::values)
            Engine::Get().GetEventManager().SetSuspend(id, true);
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
