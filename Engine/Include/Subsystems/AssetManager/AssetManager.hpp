#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "ISerializable.hpp"
#include "Shader.hpp"
#include "Debug.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"
#include "Hash.hpp"

#include <filesystem>
#include <unordered_map>
#include <memory>

namespace rge
{
    struct AssetRegistryRecord final
    {
        uid_t ID;
        size_t PathHash;

        std::filesystem::path Path;
        std::unique_ptr<RigelAsset> Asset;
    };

    class AssetManager final : public RigelSubsystem
    {
    public:
        template<typename T, typename... Args>
        NODISCARD AssetHandle<T> Load(Args&&... args)
        {
            static_assert(std::is_base_of_v<RigelAsset, T>, "T must inherit from rge::RigelAsset!");
            return AssetHandle<T>(nullptr, NULL_ID);
        }

        template<typename T>
        NODISCARD AssetHandle<T> Find(const std::filesystem::path& path)
        {
            static_assert(std::is_base_of_v<RigelAsset, T>, "T must inherit from rge::RigelAsset!");

            for (const auto& [id, record] : m_AssetsRegistry)
            {
                if (const auto cast = dynamic_cast<T*>(record.Asset.get()); cast && Hash(path.string()) == record.PathHash)
                    return AssetHandle<T>(cast, id);
            }

            // Debug::Error("Failed to find an asset of type {} at path: {}!", static_cast<std::string>(typeid(T).name()), path.string());
            return AssetHandle<T>(nullptr, NULL_ID);
        }
    INTERNAL:
        AssetManager();
        ~AssetManager() override;
    private:
        void Startup() override;
        void Shutdown() override;

        NODISCARD inline uid_t GetNextAssetID() { return m_NextAssetID++; }
        static constexpr uid_t EngineAssetsReservedSize = 1024;
        uid_t m_NextAssetID = EngineAssetsReservedSize;

        std::unordered_map<uid_t, AssetRegistryRecord> m_AssetsRegistry;

        std::filesystem::path m_WorkingDirectory;
        std::filesystem::path m_AssetsDirectory;
    };
}
