#pragma once

#include "Core.hpp"

namespace Rigel::Backend::Vulkan
{
    struct PushConstantData
    {
        glm::mat4 MVP;
        uint32_t MeshIndex;
    };
}