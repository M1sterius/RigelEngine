#pragma once

#include "Core.hpp"
#include "Debug.hpp"
#include "Math.hpp"
#include "Handles/AssetHandle.hpp"
#include "Assets/RigelAsset.hpp"
#include "Assets/Metadata/AssetMetadata.hpp"
#include "Subsystems/RigelSubsystem.hpp"
#include "Utilities/ScopeGuard.hpp"
#include "Utilities/Threading/ThreadPool.hpp"

#include <memory>
#include <atomic>
#include <filesystem>
#include <shared_mutex>

namespace Rigel
{
    class ProjectSettings;

    template<typename T>
    concept RigelAssetConcept = std::is_base_of_v<RigelAsset, T>;

    template<typename T>
    concept MetadataConcept = std::is_base_of_v<AssetMetadata, T>;

    class AssetManager final : public RigelSubsystem
    {
    private:
        struct AssetRegistryEntry final
        {
            uid_t AssetID;
            std::filesystem::path Path;
            std::unique_ptr<RigelAsset> Asset;
        };
    public:
        /**
         * @brief Synchronously loads an asset of type T.
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

            // Check if already loaded or being loaded at the moment
            if (const auto existing = FindExisting<T>(pathHash); !existing.IsNull())
                return existing;

            if (m_EnableAssetLifetimeLogging)
                Debug::Trace("AssetManager::Loading an asset at path: {}.", path.string());

            auto entry = AssetRegistryEntry{
                .AssetID = ++m_NextID,
                .Path = path,
                .Asset = MakeAsset<T>(path, m_NextID)
            };

            entry.Asset->m_IsPersistent = persistent;

            const auto rawPtr = entry.Asset.get();
            const auto handle = MakeHandle<T>(entry);

            {
                std::unique_lock lock(m_RegistryMutex);
                m_Registry[pathHash] = std::move(entry);
            }

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
                Debug::Error("Failed to load an asset at path: {}. ID: {}. Error code: {}.",
                        path.string(), rawPtr->GetID(), static_cast<int32_t>(result));
            }

            return handle;
        }

        template<RigelAssetConcept aT, MetadataConcept mT>
        AssetHandle<aT> Load(const std::filesystem::path& path, const mT* metadata, const bool persistent = false)
        {
            this->SetMetadata(path, metadata);
            return Load<aT>(path, persistent);
        }

        /**
         * @brief Asynchronously loads an asset of type T using the thread pool.
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

            // Check if already loaded or being loaded at the moment
            if (const auto existing = FindExisting<T>(pathHash); !existing.IsNull())
                return existing;

            if (m_EnableAssetLifetimeLogging)
                Debug::Trace("AssetManager::Loading an asset at path: {}.", path.string());

            auto entry = AssetRegistryEntry{
                .AssetID = ++m_NextID,
                .Path = path,
                .Asset = MakeAsset<T>(path, m_NextID)
            };

            entry.Asset->m_IsPersistent = persistent;

            const auto rawPtr = entry.Asset.get();
            const auto handle = MakeHandle<T>(entry);

            {
                std::unique_lock lock(m_RegistryMutex);
                m_Registry[pathHash] = std::move(entry);
            }

            m_ThreadPool->Enqueue([this, rawPtr, path]
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
                    Debug::Error("Failed to load an asset at path: {}. ID: {}. Error code: {}.",
                        path.string(), rawPtr->GetID(), static_cast<int32_t>(result));
                }
            });

            return handle;
        }

        template<RigelAssetConcept aT, MetadataConcept mT>
        AssetHandle<aT> LoadAsync(const std::filesystem::path& path, const mT* metadata, const bool persistent = false)
        {
            this->SetMetadata(path, metadata);
            return LoadAsync<aT>(path, persistent);
        }

        /**
         * Asynchronously unloads asset by ID.
         *
         * @note Does nothing if asset with that ID has already been unloaded or does not exist
         * @param assetID ID of an asset to unload
         */
        void Unload(const uid_t assetID);

        template<MetadataConcept T>
        NODISCARD Ref<T> GetMetadata(const std::filesystem::path& path) const
        {
            AssetMetadata* basePtr = nullptr;

            {
                std::shared_lock lock(m_MetadataMutex);

                const auto hash = Hash(path);
                if (!m_Metadata.contains(hash))
                {
                    Debug::Error("Cannot find metadata for the asset at path: {}!", path.string());
                    return nullptr;
                }

                basePtr = m_Metadata.at(hash).get();
            }

            return dynamic_cast<T*>(basePtr);
        }

        template<MetadataConcept T>
        void SetMetadata(const std::filesystem::path& path, const T* metadata)
        {
            if (!metadata)
                return;

            std::unique_lock lock(m_MetadataMutex);
            m_Metadata[Hash(path)] = std::make_unique<T>(*metadata);
        }
    INTERNAL:
        AssetManager() = default;
        ~AssetManager() override = default;

        ErrorCode Startup(const ProjectSettings& settings) override;
        ErrorCode Shutdown() override;

        NODISCARD const std::vector<std::thread::id>& GetLoadingThreadsIDs() const { return m_ThreadPool->GetThreadsIDs(); }

        void UnloadAllAssets();
    private:
        template<RigelAssetConcept T>
        NODISCARD AssetHandle<T> FindExisting(const uint64_t pathHash) const
        {
            std::shared_lock lock(m_RegistryMutex);

            if (!m_Registry.contains(pathHash))
                return AssetHandle<T>::Null();

            if (const auto cast = dynamic_cast<T*>(m_Registry.at(pathHash).Asset.get()))
                return MakeHandle<T>(m_Registry.at(pathHash));

            return AssetHandle<T>::Null();
        }

        template<RigelAssetConcept T>
        static AssetHandle<T> MakeHandle(const AssetRegistryEntry& entry)
        {
            return AssetHandle<T>(static_cast<T*>(entry.Asset.get()), entry.AssetID, std::make_shared<Backend::AssetDeleter>(entry.Asset.get()));
        }

        template<RigelAssetConcept T>
        static std::unique_ptr<RigelAsset> MakeAsset(const std::filesystem::path& path, const uid_t id)
        {
            return std::unique_ptr<RigelAsset>(static_cast<RigelAsset*>(new T(path, id)));
        }

        std::atomic<uid_t> m_NextID = 0;

        std::unordered_map<uint64_t, AssetRegistryEntry> m_Registry;
        mutable std::shared_mutex m_RegistryMutex;

        std::unordered_map<uint64_t, std::unique_ptr<AssetMetadata>> m_Metadata;
        mutable std::shared_mutex m_MetadataMutex;

        bool m_EnableAssetLifetimeLogging = true;
        std::unique_ptr<ThreadPool> m_ThreadPool;
    };
}
