#pragma once

#include "Core.hpp"
#include "RigelSubsystem.hpp"
#include "Debug.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"
#include "Hash.hpp"
#include "HandleValidator.hpp"
#include "plf_colony.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace Rigel
{
    template<typename T>
    concept RigelAssetConcept = std::is_base_of_v<RigelAsset, T>;

    struct AssetRegistryRecord final
    {
        uid_t AssetID;
        uint32_t RefCount;
        uint64_t PathHash;
        std::filesystem::path Path;
        std::unique_ptr<RigelAsset> Asset;
    };

    class AssetManager final : public RigelSubsystem
    {
    public:
        template<RigelAssetConcept T>
        NODISCARD AssetHandle<T> Find(const std::filesystem::path& path) const
        {
            std::shared_lock lock(m_RegistryMutex);

            for (const auto& record : m_AssetsRegistry)
            {
                if (const auto cast = dynamic_cast<T*>(record.Asset.get());
                    cast && Hash(path.string()) == record.PathHash)
                {
                    return AssetHandle<T>(cast, record.AssetID);
                }
            }

            return AssetHandle<T>::Null();
        }

        template<RigelAssetConcept T>
        AssetHandle<T> Load(const std::filesystem::path& path)
        {
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
                return AssetHandle<T>::Null();
            }

            const auto ID = AssignID(assetPtr.get());
            const auto rawPtr = static_cast<T*>(assetPtr.get());

            Backend::HandleValidation::HandleValidator::AddAssetHandle(ID);

            {
                std::unique_lock lock(m_RegistryMutex);
                m_AssetsRegistry.insert({
                    .AssetID = ID,
                    .RefCount = 0,
                    .PathHash = Hash(path.string()),
                    .Path = path,
                    .Asset = std::move(assetPtr)
                });
            }

            return AssetHandle<T>(rawPtr, ID);
        }

        // template<RigelAssetConcept T>
        // void Unload(const std::filesystem::path& path)
        // {
        //     const auto found = Find<T>(path);
        //
        //     if (found.IsNull())
        //     {
        //         Debug::Error("Failed to unload asset at path: {}! Asset not found!", path.string());
        //         return;
        //     }
        //
        //     // TODO: Improve unloading to work on multiple threads
        //     std::unique_lock lock(m_RegistryMutex);
        //     m_AssetsRegistry.erase(found.GetID());
        // }
        //
        template<RigelAssetConcept T>
        void Unload(const AssetHandle<T>& handle)
        {
            if (!Validate<T>(handle))
            {
                Debug::Error("Failed to unload with ID: {}! Asset not found!", handle.GetID());
                return;
            }

            // TODO: Improve unloading to work on multiple threads
            std::unique_lock lock(m_RegistryMutex);
            for (auto it = m_AssetsRegistry.begin(); it != m_AssetsRegistry.end(); ++it)
            {
                if (it->AssetID == handle.GetID())
                {
                    m_AssetsRegistry.erase(it);
                    Backend::HandleValidation::HandleValidator::RemoveAssetHandle(handle.GetID());
                }
            }
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

        NODISCARD inline bool Validate(const uid_t assetID) const
        {
            std::shared_lock lock(m_RegistryMutex);

            for (const auto& record : m_AssetsRegistry)
            {
                if (record.AssetID == assetID)
                    return true;
            }

            return false;
        }

        template<RigelAssetConcept T>
        NODISCARD inline bool Validate(const AssetHandle<T>& handle)
        {
            return Validate(handle.GetID());
        }

        NODISCARD uint32_t GetRefCount(const uid_t id);

        inline void PrintIDs()
        {
            for (const auto& record : m_AssetsRegistry)
            {
                Debug::Message("Path: {}, ID: {}, RefCount: {}", record.Path.string(), record.AssetID, record.RefCount);
            }
        }
    INTERNAL:
        AssetManager();
        ~AssetManager() override;

        void Startup() override;
        void Shutdown() override;

        void IncrementRefCount(const uid_t id);
        void DecrementRefCount(const uid_t id);

        void UnloadAllAssets();
    private:
        uid_t AssignID(RigelAsset* ptr);

        NODISCARD uid_t GetNextAssetID();
        uid_t m_NextAssetID = 1;

        mutable std::shared_mutex m_RegistryMutex;
        std::mutex m_IDMutex;

        plf::colony<AssetRegistryRecord> m_AssetsRegistry;
    };
}
