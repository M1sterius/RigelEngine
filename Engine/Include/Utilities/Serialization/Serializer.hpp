#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "nlohmann_json/json.hpp"

namespace Rigel
{
    class Serializer
    {
    public:
        static nlohmann::json Serialize(const glm::vec1 vec);
        static nlohmann::json Serialize(const glm::vec2& vec);
        static nlohmann::json Serialize(const glm::vec3& vec);
        static nlohmann::json Serialize(const glm::vec4& vec);
        static nlohmann::json Serialize(const glm::quat& quaternion);

        static glm::vec1 DeserializeVec1(const nlohmann::json& json);
        static glm::vec2 DeserializeVec2(const nlohmann::json& json);
        static glm::vec3 DeserializeVec3(const nlohmann::json& json);
        static glm::vec4 DeserializeVec4(const nlohmann::json& json);
        static glm::quat DeserializeQuaternion(const nlohmann::json& json);
    };
}
