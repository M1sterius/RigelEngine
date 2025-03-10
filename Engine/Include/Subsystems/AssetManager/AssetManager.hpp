#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "ISerializable.hpp"
#include "Asset.hpp"
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

        std::unordered_map<uid_t, std::unique_ptr<Asset>> m_AssetsRegistry;

        std::filesystem::path m_WorkingDirectory;
        std::filesystem::path m_AssetsDirectory;
    };
}
