#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "Debug.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"
#include "ThreadPool.hpp"
#include "Hash.hpp"
#include "HandleValidator.hpp"

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
            // Check if an asset has already been fully loaded
            if (const auto existing = FindExisting<T>(path); !existing.IsNull())
                return existing;

            const auto pathHash = Hash(path);

            // Check if an asset loading is in progress
            {
                std::shared_lock lock(m_LoadInProgressMutex);

                if (m_LoadInProgressMap.contains(pathHash))
                {
                    const auto handle = m_LoadInProgressMap.at(pathHash).second;
                    return handle.Cast<T>();
                }
            }


        }

        template<RigelAssetConcept T>
        AssetHandle<T> Load(const std::filesystem::path& path)
        {
            if (const auto existing = FindExisting<T>(path); !existing.IsNull())
                return existing;

            const auto assetID = m_NextID++;

            // TODO: Check if an asset is already being loaded

            std::unique_ptr<RigelAsset> uniquePtr;

            if (m_EnableAssetLifetimeLogging)
                Debug::Trace("AssetManager::Loading an asset at path: {}.", path.string());

            try
            {
                uniquePtr = std::unique_ptr<RigelAsset>(static_cast<RigelAsset*>(new T(path)));
            }
            catch (const std::exception& e)
            {
                Debug::Error("Failed to load an asset at path: {}! Exception: {}!", path.string(), e.what());
                return AssetHandle<T>::Null();
            }

            const auto rawPtr = uniquePtr.get();
            AssignAssetID(rawPtr, assetID);

            // This insures that all AssetHandle instances holding the same asset share the same reference counter
            auto refCounter = std::make_unique<std::atomic<uint32_t>>(1);
            const auto refCounterRaw = refCounter.get();

            {
                std::unique_lock lock(m_RegistryMutex);

                m_AssetsRegistry.insert({
                    .AssetID = assetID,
                    .PathHash = Hash(path),
                    .Path = path,
                    .RefCounter = std::move(refCounter),
                    .Asset = std::move(uniquePtr)
                });
            }

            return AssetHandle<T>(static_cast<T*>(rawPtr), assetID, refCounterRaw);
        }

        void Unload(const uid_t assetID)
        {
            using namespace Backend::HandleValidation;

            std::unique_ptr<RigelAsset> assetPtr;
            std::unique_ptr<std::atomic<uint32_t>> refCounterPtr;
            std::filesystem::path path;

            {
                std::unique_lock lock(m_RegistryMutex);

                for (auto it = m_AssetsRegistry.begin(); it != m_AssetsRegistry.end(); ++it)
                {
                    if (it->AssetID == assetID)
                    {
                        assetPtr = std::move(it->Asset);
                        refCounterPtr = std::move(it->RefCounter);
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
            refCounterPtr.reset();
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

        void UnloadAllAssets();

        NODISCARD inline bool IsAssetReady(const uid_t id) const
        {
            std::shared_lock lock(m_LoadInProgressMutex);

            for (const auto& [curID, _] : m_LoadInProgressMap | std::views::values)
            {
                if (curID == id)
                    return false;
            }

            return true;
        }
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

        std::atomic<uid_t> m_NextID = 1;
        bool m_EnableAssetLifetimeLogging = true;
        std::unique_ptr<ThreadPool> m_ThreadPool;

        std::unordered_map<uint64_t, std::pair<uid_t, GenericAssetHandle>> m_LoadInProgressMap;
        mutable std::shared_mutex m_LoadInProgressMutex;

        plf::colony<AssetRegistryRecord> m_AssetsRegistry;
        mutable std::shared_mutex m_RegistryMutex;
    };
}
