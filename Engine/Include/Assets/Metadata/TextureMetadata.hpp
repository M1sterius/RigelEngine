#pragma once

#include "Core.hpp"
#include "AssetMetadata.hpp"

#include <filesystem>

namespace Rigel
{
    struct TextureMetadata : public AssetMetadata
    {
        std::filesystem::path Path;
        int32_t RequiredComponents;

        void* Pixels;
        uint32_t Width;
        uint32_t Height;
        int32_t Components;
    };
}