#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"

#include <filesystem>

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

        std::filesystem::path m_WorkingDirectory;
        std::filesystem::path m_AssetsDirectory;
    };
}