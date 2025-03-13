#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
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
        size_t PathHash;
        std::filesystem::path Path;
        std::unique_ptr<RigelAsset> Asset;
    };

    class AssetManager final : public RigelSubsystem
    {
    public:
        template<typename T>
        NODISCARD AssetHandle<T> Load(const std::filesystem::path& path)
        {
            static_assert(std::is_base_of_v<RigelAsset, T>, "T must inherit from rge::RigelAsset!");

            if (const auto found = Find<T>(path); !found.IsNull())
                return found;

            RigelAsset* assetPtr = nullptr;

            try {
                assetPtr = static_cast<RigelAsset*>(new T(path));
            }
            catch (const std::exception& e)
            {
                Debug::Error("Failed to load an asset at path: {}! Exception: {}!", path.string(), e.what());
                return AssetHandle<T>(nullptr, NULL_ID);
            }

            const auto ID = AssignID(assetPtr);

            m_AssetsRegistry[ID] = {
                .PathHash = Hash(path.string()),
                .Path = path,
                .Asset = std::unique_ptr<RigelAsset>(assetPtr)
            };

            return AssetHandle<T>(static_cast<T*>(assetPtr), ID);
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

            return AssetHandle<T>(nullptr, NULL_ID);
        }

        template<typename T>
        void Unload(const std::filesystem::path& path)
        {
            static_assert(std::is_base_of_v<RigelAsset, T>, "T must inherit from rge::RigelAsset!");

            const auto found = Find<T>(path);

            if (found.IsNull())
            {
                Debug::Error("Failed to unload asset at path: {}! Asset not found!", path.string());
                return;
            }

            m_AssetsRegistry.erase(found.GetID());
        }

        template<typename T>
        void Unload(const AssetHandle<T>& handle)
        {
            static_assert(std::is_base_of_v<RigelAsset, T>, "T must inherit from rge::RigelAsset!");

            if (!Validate<T>(handle))
            {
                Debug::Error("Failed to unload with ID: {}! Asset not found!", handle.GetID());
                return;
            }

            m_AssetsRegistry.erase(handle.GetID());
        }

        NODISCARD inline bool Validate(const uid_t assetID) const
        {
            return m_AssetsRegistry.contains(assetID);
        }

        template<typename T>
        NODISCARD inline bool Validate(const AssetHandle<T>& handle)
        {
            return Validate(handle.GetID());
        }
    INTERNAL:
        AssetManager();
        ~AssetManager() override;
    private:
        void Startup() override;
        void Shutdown() override;

        uid_t AssignID(RigelAsset* ptr);

        NODISCARD inline uid_t GetNextAssetID() { return m_NextAssetID++; }
        static constexpr uid_t EngineAssetsReservedSize = 1024;
        uid_t m_NextAssetID = EngineAssetsReservedSize;

        std::unordered_map<uid_t, AssetRegistryRecord> m_AssetsRegistry;

        std::filesystem::path m_WorkingDirectory;
        std::filesystem::path m_AssetsDirectory;
    };
}
