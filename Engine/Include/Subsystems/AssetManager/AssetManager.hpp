#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "Debug.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"
#include "ThreadPool.hpp"
#include "Hash.hpp"
#include "HandleValidator.hpp"
#include "ScopeGuard.hpp"

#include "plf_colony.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <atomic>
#include <shared_mutex>

namespace Rigel
{
    class ProjectSettings;

    template<typename T>
    concept RigelAssetConcept = std::is_base_of_v<RigelAsset, T>;

    struct AssetRegistryEntry final
    {
        uid_t AssetID;
        uint64_t PathHash;
        std::filesystem::path Path;
        std::unique_ptr<std::atomic<uint32_t>> RefCounter;
        std::unique_ptr<RigelAsset> Asset;
    };

    class AssetManager final : public RigelSubsystem
    {
    public:
        template<RigelAssetConcept T>
        AssetHandle<T> LoadAsync(const std::filesystem::path& path, const bool persistent = false)
        {
            const auto pathHash = Hash(path);

            // Already loaded
            if (const auto existing = FindExisting<T>(pathHash); !existing.IsNull())
                return existing;

            // Load in-progress
            {
                std::unique_lock lock(m_LoadInProgressMutex);
                if (m_LoadInProgressMap.contains(pathHash))
                    return m_LoadInProgressMap.at(pathHash).Cast<T>();
            }

            if (m_EnableAssetLifetimeLogging)
                Debug::Trace("AssetManager::Loading an asset at path: {}.", path.string());

            auto entry = AssetRegistryEntry{
                .AssetID = ++m_NextID,
                .PathHash = pathHash,
                .Path = path,
                .RefCounter = std::make_unique<std::atomic<uint32_t>>(persistent ? 2 : 1),
                .Asset = MakeAsset<T>(path, m_NextID)
            };

            const auto rawPtr = entry.Asset.get();
            const auto handle = MakeHandle<T>(entry);

            {
                std::unique_lock lock(m_LoadInProgressMutex);
                m_LoadInProgressMap[pathHash] = handle.ToGeneric();
            }

            {
                std::unique_lock lock(m_RegistryMutex);
                m_AssetsRegistry.insert(std::move(entry));
            }

            m_ThreadPool->Enqueue([this, rawPtr, path, pathHash]
            {
                auto loadFinishedGuard = ScopeGuard([rawPtr]() { rawPtr->m_LoadFinished = true; });

                if (const auto result = rawPtr->Init(); result != ErrorCode::OK)
                {
                    Debug::Error("Failed to load an asset at path: {}. ID: {}! Error code: {}.",
                        path.string(), static_cast<int32_t>(result), rawPtr->GetID());
                }

                std::unique_lock lock(m_LoadInProgressMutex);
                m_LoadInProgressMap.erase(pathHash);
            });

            return handle;
        }

        template<RigelAssetConcept T>
        AssetHandle<T> Load(const std::filesystem::path& path, const bool persistent = false)
        {
            const auto pathHash = Hash(path);

            // Check if already loaded
            if (const auto existing = FindExisting<T>(pathHash); !existing.IsNull())
                return existing;

            if (m_EnableAssetLifetimeLogging)
                Debug::Trace("AssetManager::Loading an asset at path: {}.", path.string());

            auto entry = AssetRegistryEntry{
                .AssetID = ++m_NextID,
                .PathHash = pathHash,
                .Path = path,
                .RefCounter = std::make_unique<std::atomic<uint32_t>>(persistent ? 2 : 1),
                .Asset = MakeAsset<T>(path, m_NextID)
            };

            const auto rawPtr = entry.Asset.get();
            auto loadFinishedGuard = ScopeGuard([rawPtr]() { rawPtr->m_LoadFinished = true; });
            const auto handle = MakeHandle<T>(entry);

            {
                std::unique_lock lock(m_RegistryMutex);
                m_AssetsRegistry.insert(std::move(entry));
            }

            if (const auto result = rawPtr->Init(); result != ErrorCode::OK)
            {
                Debug::Error("Failed to load an asset at path: {}. ID: {}! Error code: {}.",
                    path.string(), static_cast<int32_t>(result), rawPtr->GetID());
            }

            return handle;
        }

        inline void Unload(const uid_t assetID)
        {
            m_ThreadPool->Enqueue([this, assetID] { this->UnloadImpl(assetID); });
        }

        NODISCARD std::filesystem::path GetAssetPath(const uid_t id) const
        {
            for (const auto& record : m_AssetsRegistry)
            {
                if (record.AssetID == id)
                    return record.Path;
            }

            Debug::Error("Failed to find an asset with ID: {}!", id);
            return "";
        }
    INTERNAL:
        AssetManager() = default;
        ~AssetManager() override = default;

        ErrorCode Startup(const ProjectSettings& settings) override;
        ErrorCode Shutdown() override;

        NODISCARD const std::vector<std::thread::id>& GetLoadingThreadIDs() const { return m_ThreadPool->GetThreadIDs(); }

        ErrorCode PreloadAssets();
        void UnloadAllAssets();
    private:
        template<RigelAssetConcept T>
        NODISCARD AssetHandle<T> FindExisting(const uint64_t pathHash) const
        {
            std::shared_lock lock(m_RegistryMutex);

            for (const auto& entry : m_AssetsRegistry)
            {
                if (const auto cast = dynamic_cast<T*>(entry.Asset.get());
                    cast && pathHash == entry.PathHash)
                {
                    return MakeHandle<T>(entry);
                }
            }

            return AssetHandle<T>::Null();
        }

        template<RigelAssetConcept T>
        static AssetHandle<T> MakeHandle(const AssetRegistryEntry& entry)
        {
            return AssetHandle<T>(static_cast<T*>(entry.Asset.get()), entry.AssetID, entry.RefCounter.get());
        }

        template<RigelAssetConcept T>
        static std::unique_ptr<RigelAsset> MakeAsset(const std::filesystem::path& path, const uid_t id)
        {
            return std::unique_ptr<RigelAsset>(static_cast<RigelAsset*>(new T(path, id)));
        }

        void UnloadImpl(const uid_t assetID);

        std::atomic<uid_t> m_NextID = 0;
        bool m_EnableAssetLifetimeLogging = true;
        std::unique_ptr<ThreadPool> m_ThreadPool;

        plf::colony<AssetRegistryEntry> m_AssetsRegistry;
        mutable std::shared_mutex m_RegistryMutex;

        std::unordered_map<uint64_t, GenericAssetHandle> m_LoadInProgressMap;
        mutable std::shared_mutex m_LoadInProgressMutex;
    };
}
