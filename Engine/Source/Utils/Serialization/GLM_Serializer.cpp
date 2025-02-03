#include "GLM_Serializer.hpp"
#include "json.hpp"

namespace rge
{

    nlohmann::json GLM_Serializer::Serialize(const glm::vec1 vec)
    {
        nlohmann::json json = {
            {"x", vec.x}
        };
        return json;
    }

    nlohmann::json GLM_Serializer::Serialize(const glm::vec2& vec)
    {
        nlohmann::json json = {
            {"x", vec.x},
            {"y", vec.y}
        };
        return json;
    }

    nlohmann::json GLM_Serializer::Serialize(const glm::vec3& vec)
    {
        nlohmann::json json = {
            {"x", vec.x},
            {"y", vec.y},
            {"z", vec.z}
        };
        return json;
    }

    nlohmann::json GLM_Serializer::Serialize(const glm::vec4& vec)
    {
        nlohmann::json json = {
            {"x", vec.x},
            {"y", vec.y},
            {"z", vec.z},
            {"w", vec.w}
        };
        return json;
    }

    nlohmann::json GLM_Serializer::Serialize(const glm::quat& quaternion)
    {
        return Serialize(glm::vec4(quaternion.x, quaternion.y, quaternion.z, quaternion.w));
    }

    glm::vec1 GLM_Serializer::DeserializeVec1(const nlohmann::json& json)
    {
        return glm::vec1(json.value("x", 0.0f));
    }

    glm::vec2 GLM_Serializer::DeserializeVec2(const nlohmann::json& json)
    {
        return {json.value("x", 0.0f), json.value("y", 0.0f)};
    }

    glm::vec3 GLM_Serializer::DeserializeVec3(const nlohmann::json& json)
    {
        return {json.value("x", 0.0f), json.value("y", 0.0f), json.value("z", 0.0f)};
    }

    glm::vec4 GLM_Serializer::DeserializeVec4(const nlohmann::json& json)
    {
        return {json.value("x", 0.0f), json.value("y", 0.0f), json.value("z", 0.0f), json.value("w", 0.0f)};
    }

    glm::quat GLM_Serializer::DeserializeQuaternion(const nlohmann::json& json)
    {
        return {json.value("x", 0.0f), json.value("y", 0.0f), json.value("z", 0.0f), json.value("w", 0.0f)};
    }
}
