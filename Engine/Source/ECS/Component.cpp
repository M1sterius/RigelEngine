#include "Component.hpp"
#include "Debug.hpp"
#include "json.hpp"

namespace rge
{
    // The NULL_ID will be overwritten by GameObject::AddComponent method
    Component::Component() : RigelObject(NULL_ID)
    {

    }

    nlohmann::json Component::Serialize() const
    {
        auto json = nlohmann::json();
        json["Type"] = GetTypeName();
        json["ID"] = GetID();
        return json;
    }

    void Component::Deserialize(const nlohmann::json& json)
    {
        OverrideID(json["ID"].get<uid_t>());
    }
}
