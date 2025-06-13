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

    struct AssetRegistryRecord final
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
        AssetHandle<T> LoadAsync(const std::filesystem::path& path)
        {
            // Returns handle if the asset is already FULLY LOADED.
            if (const auto existing = FindExisting<T>(path); !existing.IsNull())
                return existing;

            const auto pathHash = Hash(path);

            // Returns handle if an asset is BEING LOADED at the moment
            if (m_LoadInProgressMap.contains(pathHash))
                return m_LoadInProgressMap.at(pathHash).Cast<T>();

            if (m_EnableAssetLifetimeLogging)
                Debug::Trace("AssetManager::Loading an asset at path: {}.", path.string());

            auto assetPtr = static_cast<RigelAsset*>(new T(path));

            const auto assetID = ++m_NextID;
            AssignAssetID(assetPtr, assetID);

            auto refCounter = std::make_unique<std::atomic<uint32_t>>(1);
            const auto refCounterRaw = refCounter.get();

            const auto handle = AssetHandle<T>(static_cast<T*>(assetPtr), assetID, refCounterRaw);

            // Add to in-progress map
            {
                std::unique_lock lock(m_LoadInProgressMutex);
                m_LoadInProgressMap[pathHash] = handle.template Cast<RigelAsset>();
            }

            auto future = m_ThreadPool->Enqueue([this, assetPtr, path, pathHash]()
            {
                auto guard = ScopeGuard([assetPtr] { assetPtr->m_LoadFinished = true; });

                if (const auto result = assetPtr->Init(); result != ErrorCode::OK)
                {
                    Debug::Error("Failed to load an asset at path: {}! Error code: {}.", path.string(), static_cast<int32_t>(result));
                    return;
                }

                {
                    std::unique_lock lock(m_LoadInProgressMutex);
                    m_LoadInProgressMap.erase(pathHash);
                }
            });

            {
                std::unique_lock lock(m_RegistryMutex);

                m_AssetsRegistry.insert({
                    .AssetID = assetID,
                    .PathHash = pathHash,
                    .Path = path,
                    .RefCounter = std::move(refCounter),
                    .Asset = std::unique_ptr<RigelAsset>(assetPtr)
                });
            }

            return handle;
        }

        template<RigelAssetConcept T>
        AssetHandle<T> Load(const std::filesystem::path& path)
        {
            // Returns handle if the asset is already FULLY loaded.
            if (const auto existing = FindExisting<T>(path); !existing.IsNull())
                return existing;

            if (m_EnableAssetLifetimeLogging)
                Debug::Trace("AssetManager::Loading an asset at path: {}.", path.string());

            const auto pathHash = Hash(path);
            const auto assetID = ++m_NextID;

            auto assetPtr = std::unique_ptr<RigelAsset>(static_cast<RigelAsset*>(new T(path)));
            const auto rawAssetPtr = assetPtr.get();

            auto loadFinishedGuard = ScopeGuard([rawAssetPtr]() { rawAssetPtr->m_LoadFinished = true; });

            if (const auto result = assetPtr->Init(); result != ErrorCode::OK)
            {
                Debug::Error("Failed to load an asset at path: {}! Error code: {}.", path.string(), static_cast<int32_t>(result));
                return AssetHandle<T>::Null();
            }

            AssignAssetID(rawAssetPtr, assetID);

            // This insures that all AssetHandle instances holding the same asset share the same reference counter
            auto refCounter = std::make_unique<std::atomic<uint32_t>>(1);
            const auto refCounterRaw = refCounter.get();

            {
                std::unique_lock lock(m_RegistryMutex);

                m_AssetsRegistry.insert({
                    .AssetID = assetID,
                    .PathHash = pathHash,
                    .Path = path,
                    .RefCounter = std::move(refCounter),
                    .Asset = std::move(assetPtr)
                });
            }

            return AssetHandle<T>(static_cast<T*>(rawAssetPtr), assetID, refCounterRaw);
        }

        void Unload(const uid_t assetID)
        {
            using namespace Backend::HandleValidation;

            std::unique_ptr<RigelAsset> assetPtr;
            std::filesystem::path path;

            {
                std::unique_lock lock(m_RegistryMutex);

                for (auto it = m_AssetsRegistry.begin(); it != m_AssetsRegistry.end(); ++it)
                {
                    if (it->AssetID == assetID)
                    {
                        assetPtr = std::move(it->Asset);
                        path = std::move(it->Path);

                        m_AssetsRegistry.erase(it);
                        break;
                    }
                }
            }

            if (m_EnableAssetLifetimeLogging)
                Debug::Trace("AssetManager::Destroying an asset at path: {}.", path.string());

            HandleValidator::RemoveHandle<HandleType::AssetHandle>(assetID);
            assetPtr.reset(); // explicitly delete the object just for clarity
        }

        template<RigelAssetConcept T>
        NODISCARD std::filesystem::path GetAssetPath(const AssetHandle<T>& handle) const
        {
            for (const auto& record : m_AssetsRegistry)
            {
                if (record.AssetID == handle.GetID())
                    return record.Path;
            }

            Debug::Error("Failed to find an asset with ID: {}!", handle.GetID());
            return "";
        }
    INTERNAL:
        AssetManager() = default;
        ~AssetManager() override = default;

        ErrorCode Startup(const ProjectSettings& settings) override;
        ErrorCode Shutdown() override;

        NODISCARD const std::vector<std::thread::id>& GetLoadingThreadIDs() const { return m_ThreadPool->GetThreadIDs(); }

        void UnloadAllAssets();
    private:
        template<RigelAssetConcept T>
        NODISCARD AssetHandle<T> FindExisting(const std::filesystem::path& path) const
        {
            std::shared_lock lock(m_RegistryMutex);

            for (const auto& record : m_AssetsRegistry)
            {
                if (const auto cast = dynamic_cast<T*>(record.Asset.get());
                    cast && Hash(path.string()) == record.PathHash)
                {
                    return AssetHandle<T>(cast, record.AssetID, record.RefCounter.get());
                }
            }

            return AssetHandle<T>::Null();
        }

        void AssignAssetID(RigelAsset* ptr, const uid_t id);

        std::atomic<uid_t> m_NextID = 0;
        bool m_EnableAssetLifetimeLogging = true;
        std::unique_ptr<ThreadPool> m_ThreadPool;

        std::unordered_map<uint64_t, GenericAssetHandle> m_LoadInProgressMap;
        mutable std::shared_mutex m_LoadInProgressMutex;

        plf::colony<AssetRegistryRecord> m_AssetsRegistry;
        mutable std::shared_mutex m_RegistryMutex;
    };
}
