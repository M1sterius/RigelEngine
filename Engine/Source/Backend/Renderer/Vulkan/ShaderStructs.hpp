#pragma once

#include "Core.hpp"

namespace Rigel::Backend::Vulkan
{
    struct PushConstantData
    {
        glm::mat4 MVP;
        uint32_t MeshIndex;
    };

    struct MeshData
    {
        uint32_t DiffuseIndex;
        uint32_t SpecularIndex;
        uint32_t NormalIndex;
    };
}