#pragma once

#include "json_fwd.hpp"
#include "Utils/HeaderUtils/HeaderUtils.hpp"

namespace rge
{
    class ISerializable
    {
    public:
        virtual ~ISerializable() = default;

        NODISCARD virtual nlohmann::json Serialize() const = 0;
        virtual void Deserialize(const nlohmann::json& json) = 0;
    };
}
