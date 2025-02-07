#include "Component.hpp"
#include "Debug.hpp"
#include "UIDGenerator.hpp"
#include "json.hpp"

namespace rge
{
    Component::Component() : RigelObject(UIDGenerator::Generate())
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

    }
}
