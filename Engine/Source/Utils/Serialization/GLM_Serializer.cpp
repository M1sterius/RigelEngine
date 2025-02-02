#include "Utils/Serialization/GLM_Serializer.hpp"
#include "json.hpp"

namespace rge
{

    nlohmann::json GLM_Serializer::Serialize(const glm::vec3& obj)
    {
        auto json = nlohmann::json();
        json = {
            {"x", obj.x},
            {"y", obj.y},
            {"z", obj.z}
        };
        return json;
    }

    glm::vec3 GLM_Serializer::Deserialize(const nlohmann::json& json)
    {
        return {json["x"].get<float32_t>(), json["y"].get<float32_t>(), json["z"].get<float32_t>()};
    }
}
