#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <string>
#include <memory>
#include <optional>

namespace Rigel
{
    struct AssetRegistryEntry final
    {
        uid_t AssetID;
        uint64_t PathHash;
        std::filesystem::path Path;
        std::unique_ptr<std::atomic<uint32_t>> RefCounter;
        std::unique_ptr<RigelAsset> Asset;
    };
}