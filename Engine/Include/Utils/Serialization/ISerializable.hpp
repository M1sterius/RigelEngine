#pragma once

#include "json_fwd.hpp"
#include "Core.hpp"

namespace Rigel
{
    class ISerializable
    {
    public:
        virtual ~ISerializable() = default;

        NODISCARD virtual nlohmann::json Serialize() const = 0;

        // Return true if the deserialization was successful, false otherwise
        virtual bool Deserialize(const nlohmann::json& json) = 0;
    };
}
