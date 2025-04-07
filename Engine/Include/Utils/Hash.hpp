#pragma once

#include "Core.hpp"

#include <functional>
#include <string>

namespace Rigel
{
    NODISCARD inline uint64_t Hash(const std::string& string)
    {
        static auto hashFunc = std::hash<std::string>();
        return hashFunc(string);
    }
}