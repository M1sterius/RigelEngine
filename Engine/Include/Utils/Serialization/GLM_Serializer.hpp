#pragma once

#include "Utils/HeaderUtils/HeaderUtils.hpp"
#include "json_fwd.hpp"
#include "glm.hpp"

namespace rge
{
    class RGE_API GLM_Serializer
    {
    public:
        static nlohmann::json Serialize(const glm::vec3& obj);

        static glm::vec3 Deserialize(const nlohmann::json& json);
    };
}
