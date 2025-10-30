#pragma once

#include <cmath>
#include <string>
#include <filesystem>

#include "Random.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE // required for vulkan
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"

namespace Rigel
{
    class Math
    {
    public:
        NODISCARD static uint64_t Hash(const std::string& string);
        NODISCARD static uint64_t Hash(const std::filesystem::path& path);
    };
}
