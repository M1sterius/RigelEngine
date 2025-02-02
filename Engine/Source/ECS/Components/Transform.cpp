#include "ECS/Components/Transform.hpp"
#include "json.hpp"


namespace rge
{
    nlohmann::json Transform::Serialize() const
    {
        return {};
    }

    void Transform::Deserialize(const nlohmann::json& json)
    {

    }

    Transform::Transform() : Component()
    {

    }
}
