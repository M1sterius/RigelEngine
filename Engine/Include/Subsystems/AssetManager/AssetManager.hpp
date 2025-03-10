#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "ISerializable.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"

#include <filesystem>
#include <unordered_map>
#include <memory>

namespace rge
{
    class AssetManager final : public RigelSubsystem
    {
    public:
    INTERNAL:
        AssetManager();
        ~AssetManager() override;
    private:
        void Startup() override;
        void Shutdown() override;

        std::unordered_map<uid_t, std::unique_ptr<RigelAsset>> m_AssetsRegistry; // The asset manager owns all assets

        std::filesystem::path m_WorkingDirectory;
        std::filesystem::path m_AssetsDirectory;
    };
}
