#pragma once

#include "json_fwd.hpp"

namespace rge
{
    class ISerializable
    {
    public:
        virtual void Serialize(nlohmann::json& json) = 0;
        virtual void Deserialize(const nlohmann::json& json) = 0;
    };
}
