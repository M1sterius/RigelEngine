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
#include <mutex>
#include <shared_mutex>

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
        NODISCARD AssetHandle<T> Find(const std::filesystem::path& path) const
        {
            static_assert(std::is_base_of_v<RigelAsset, T>, "T must inherit from rge::RigelAsset!");

            std::shared_lock lock(m_RegistryMutex);

            for (const auto& [id, record] : m_AssetsRegistry)
            {
                if (const auto cast = dynamic_cast<T*>(record.Asset.get()); cast && Hash(path.string()) == record.PathHash)
                    return AssetHandle<T>(cast, id);
            }

            return AssetHandle<T>(nullptr, NULL_ID);
        }

        template<typename T>
        NODISCARD AssetHandle<T> Load(const std::filesystem::path& path)
        {
            static_assert(std::is_base_of_v<RigelAsset, T>, "T must inherit from rge::RigelAsset!");

            if (const auto found = Find<T>(path); !found.IsNull())
                return found;

            std::unique_ptr<RigelAsset> assetPtr;

            try
            {
                assetPtr = std::unique_ptr<RigelAsset>(static_cast<RigelAsset*>(new T(path)));
            }
            catch (const std::exception& e)
            {
                Debug::Error("Failed to load an asset at path: {}! Exception: {}!", path.string(), e.what());
                return AssetHandle<T>(nullptr, NULL_ID);
            }

            const auto ID = AssignID(assetPtr.get());

            {
                std::unique_lock lock(m_RegistryMutex);
                m_AssetsRegistry[ID] = {
                    .PathHash = Hash(path.string()),
                    .Path = path,
                    .Asset = std::move(assetPtr)
                };
            }

            return AssetHandle<T>(static_cast<T*>(assetPtr.get()), ID);
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

            // TODO: Improve unloading to work on multiple threads
            std::unique_lock lock(m_RegistryMutex);
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

            // TODO: Improve unloading to work on multiple threads
            std::unique_lock lock(m_RegistryMutex);
            m_AssetsRegistry.erase(handle.GetID());
        }

        NODISCARD inline bool Validate(const uid_t assetID) const
        {
            std::shared_lock lock(m_RegistryMutex);
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

        void LoadEngineAssets();

        uid_t AssignID(RigelAsset* ptr);

        NODISCARD uid_t GetNextAssetID();
        uid_t m_NextAssetID = 1;

        mutable std::shared_mutex m_RegistryMutex;
        std::mutex m_IDMutex;

        std::unordered_map<uid_t, AssetRegistryRecord> m_AssetsRegistry;

        std::filesystem::path m_WorkingDirectory;
        std::filesystem::path m_AssetsDirectory;
    };
}
