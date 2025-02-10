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

    bool Component::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("ID"))
        {
            Debug::Error("Failed to serialize component!");
            return false;
        }

        OverrideID(json["ID"].get<uid_t>());

        return true;
    }
}
