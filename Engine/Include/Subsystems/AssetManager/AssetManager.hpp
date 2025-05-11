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
            using namespace Backend::HandleValidation;

            if (const auto found = Find<T>(path); !found.IsNull())
                return found;

            std::unique_ptr<RigelAsset> unqPtr;

            try {
                unqPtr = std::unique_ptr<RigelAsset>(static_cast<RigelAsset*>(new T(path)));
            }
            catch (const std::exception& e)
            {
                Debug::Error("Failed to load an asset at path: {}! Exception: {}!", path.string(), e.what());
                return AssetHandle<T>::Null();
            }

            const auto rawPtr = unqPtr.get();
            const auto ID = AssignID(rawPtr);

            HandleValidator::AddHandle<HandleType::AssetHandle>(ID);

            {
                std::unique_lock lock(m_RegistryMutex);

                m_AssetsRegistry.insert({
                    .AssetID = ID,
                    .RefCount = 0,
                    .PathHash = Hash(path.string()),
                    .Path = path,
                    .Asset = std::move(unqPtr)
                });
            }

            return AssetHandle<T>(static_cast<T*>(rawPtr), ID);
        }

        template<RigelAssetConcept T>
        void Unload(const AssetHandle<T>& handle)
        {
            using namespace Backend::HandleValidation;

            std::unique_ptr<RigelAsset> ptr;

            {
                std::unique_lock lock(m_RegistryMutex);

                for (auto it = m_AssetsRegistry.begin(); it != m_AssetsRegistry.end(); ++it)
                {
                    if (it->AssetID == handle.GetID())
                    {
                        ptr = std::move(it->Asset);
                        m_AssetsRegistry.erase(it);
                        break;
                    }
                }
            }

            HandleValidator::RemoveHandle<HandleType::AssetHandle>(handle.GetID());
            ptr.reset(); // explicitly delete the object just for clarity
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

        NODISCARD uint32_t GetRefCount(const uid_t id);
    INTERNAL:
        AssetManager() = default;
        ~AssetManager() override = default;

        int32_t Startup() override;
        int32_t Shutdown() override;

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
