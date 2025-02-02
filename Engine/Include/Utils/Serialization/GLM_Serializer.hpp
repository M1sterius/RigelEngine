#pragma once

#include "json.hpp"
#include "Utils/HeaderUtils/HeaderUtils.hpp"
//#include "json_fwd.hpp"
#include "glm.hpp"

namespace rge
{
    class RGE_API GLM_Serializer
    {
    public:
        template <typename T, size_t SIZE>
        static nlohmann::json Serialize(const T& obj)
        {
            nlohmann::json json;
            if constexpr (SIZE >= 1) json["x"] = obj.x;
            if constexpr (SIZE >= 2) json["y"] = obj.y;
            if constexpr (SIZE >= 3) json["z"] = obj.z;
            if constexpr (SIZE >= 4) json["w"] = obj.w;
            return json;
        }

        template <typename T, size_t SIZE>
        static T Deserialize(const nlohmann::json& json)
        {
            T obj;
            if constexpr (SIZE >= 1) obj.x = json.value("x", 0.0f);
            if constexpr (SIZE >= 2) obj.y = json.value("y", 0.0f);
            if constexpr (SIZE >= 3) obj.z = json.value("z", 0.0f);
            if constexpr (SIZE >= 4) obj.w = json.value("w", 0.0f);
            return obj;
        }
    };
}
