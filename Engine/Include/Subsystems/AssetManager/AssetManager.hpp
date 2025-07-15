#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "Debug.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"
#include "AssetMetadata.hpp"
#include "ThreadPool.hpp"
#include "Hash.hpp"
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

    class AssetManager final : public RigelSubsystem
    {
    private:
        struct AssetRegistryEntry final
        {
            uid_t AssetID;
            uint64_t PathHash;
            std::filesystem::path Path;
            std::unique_ptr<std::atomic<uint32_t>> RefCounter;
            std::unique_ptr<RigelAsset> Asset;
        };
    public:
        /**
         * @brief Asynchronously loads an asset of type T from disk using the thread pool.
         *
         * This function constructs the asset and queues its `Init()` call on a background thread.
         * It immediately returns an AssetHandle, which will point to the asset, even if initialization is not yet complete.
         *
         * If the asset is already loaded or is currently being loaded, the existing handle is returned.
         *
         * @tparam T Type of the asset to load. Must satisfy the RigelAssetConcept.
         * @param path Filesystem path to the asset.
         * @param persistent If true, the asset will not be automatically deleted when its reference count reaches 0.
         * @return AssetHandle<T> Handle to the asset. You can check `.IsReady()` to check if the asset is loaded.
         * You can use `.WaitReady()` to stall until the loading is finished.
         */
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
                auto loadFinishedGuard = ScopeGuard([rawPtr]
                {
                    {
                        std::unique_lock lock(rawPtr->m_CvMutex);
                        rawPtr->m_LoadFinished = true;
                    }

                    rawPtr->m_CV.notify_one();
                });

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

        /**
         * @brief Synchronously loads an asset of type T from disk.
         *
         * This function will immediately load the asset by constructing it, initializing it via `Init()`,
         * and registering it in the asset registry. If the asset is already loaded, it returns the existing handle.
         *
         * @tparam T Type of the asset to load. Must satisfy the RigelAssetConcept.
         * @param path Filesystem path to the asset.
         * @param persistent If true, the asset will not be automatically deleted when its reference count reaches 0.
         * @return AssetHandle<T> Handle to the loaded asset. May point to an uninitialized asset if loading failed.
         */
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
            const auto handle = MakeHandle<T>(entry);
            auto loadFinishedGuard = ScopeGuard([rawPtr]
            {
                {
                    std::unique_lock lock(rawPtr->m_CvMutex);
                    rawPtr->m_LoadFinished = true;
                }

                rawPtr->m_CV.notify_one();
            });

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

        void SetAssetMetadata(const std::filesystem::path& path, const std::shared_ptr<AssetMetadata>& metadata)
        {
            std::unique_lock lock(m_MetadataMutex);
            m_Metadata[path] = metadata;
        }

        template<typename T>
        NODISCARD Result<std::shared_ptr<T>> GetAssetMetadata(const std::filesystem::path& path)
        {
            std::shared_ptr<T> retPtr;

            {
                std::unique_lock lock(m_MetadataMutex);
                if (!m_Metadata.contains(path))
                {
                    return Result<std::shared_ptr<T>>::Error(ErrorCode::ASSET_METADATA_NOT_FOUND);
                }

                retPtr = std::dynamic_pointer_cast<T>(m_Metadata.at(path));
            }

            if (!retPtr)
            {
                return Result<std::shared_ptr<T>>::Error(ErrorCode::DYNAMIC_CAST_ERROR);
            }

            return Result<std::shared_ptr<T>>::Ok(retPtr);
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

        std::unordered_map<std::filesystem::path, std::shared_ptr<AssetMetadata>> m_Metadata;
        mutable std::mutex m_MetadataMutex;
    };
}
