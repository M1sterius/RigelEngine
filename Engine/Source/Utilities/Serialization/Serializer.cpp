#include "Utilities/Serialization/Serializer.hpp"

#include "nlohmann_json/json.hpp"

namespace Rigel
{

    nlohmann::json Serializer::Serialize(const glm::vec1 vec)
    {
        nlohmann::json json = {
            {"x", vec.x}
        };
        return json;
    }

    nlohmann::json Serializer::Serialize(const glm::vec2& vec)
    {
        nlohmann::json json = {
            {"x", vec.x},
            {"y", vec.y}
        };
        return json;
    }

    nlohmann::json Serializer::Serialize(const glm::vec3& vec)
    {
        nlohmann::json json = {
            {"x", vec.x},
            {"y", vec.y},
            {"z", vec.z}
        };
        return json;
    }

    nlohmann::json Serializer::Serialize(const glm::vec4& vec)
    {
        nlohmann::json json = {
            {"x", vec.x},
            {"y", vec.y},
            {"z", vec.z},
            {"w", vec.w}
        };
        return json;
    }

    nlohmann::json Serializer::Serialize(const glm::quat& quaternion)
    {
        return Serialize(glm::vec4(quaternion.x, quaternion.y, quaternion.z, quaternion.w));
    }

    glm::vec1 Serializer::DeserializeVec1(const nlohmann::json& json)
    {
        return glm::vec1(json.value("x", 0.0f));
    }

    glm::vec2 Serializer::DeserializeVec2(const nlohmann::json& json)
    {
        return {json.value("x", 0.0f), json.value("y", 0.0f)};
    }

    glm::vec3 Serializer::DeserializeVec3(const nlohmann::json& json)
    {
        return {json.value("x", 0.0f), json.value("y", 0.0f), json.value("z", 0.0f)};
    }

    glm::vec4 Serializer::DeserializeVec4(const nlohmann::json& json)
    {
        return {json.value("x", 0.0f), json.value("y", 0.0f), json.value("z", 0.0f), json.value("w", 0.0f)};
    }

    glm::quat Serializer::DeserializeQuaternion(const nlohmann::json& json)
    {
        // The order of elements in the quaternion is different from vec4 you fucking idiot!!!
        return {json.value("w", 0.0f), json.value("x", 0.0f), json.value("y", 0.0f), json.value("z", 0.0f)};
    }
}
