#pragma once

#include "Core.hpp"
#include "AssetMetadata.hpp"

#include <filesystem>

namespace Rigel
{
    struct TextureMetadata : public AssetMetadata
    {
        std::filesystem::path Path{};
        unsigned char* Pixels{nullptr};

        uint32_t Width{0};
        uint32_t Height{0};
        int32_t Components{0};
        bool Linear{false};
    };
}
