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
        AssetHandle<T> Load(const std::filesystem::path& path)
        {
            if (const auto existing = FindExisting<T>(path); !existing.IsNull())
                return existing;

            std::unique_ptr<RigelAsset> uniquePtr;

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
            const auto ID = AssignID(rawPtr);

            // This insures that all AssetHandle instances holding the same asset share the same reference counter
            auto refCounter = std::make_unique<std::atomic<uint32_t>>(1);
            const auto refCounterRaw = refCounter.get();

            {
                std::unique_lock lock(m_RegistryMutex);

                m_AssetsRegistry.insert({
                    .AssetID = ID,
                    .PathHash = Hash(path),
                    .Path = path,
                    .RefCounter = std::move(refCounter),
                    .Asset = std::move(uniquePtr)
                });
            }

            return AssetHandle<T>(static_cast<T*>(rawPtr), ID, refCounterRaw);
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

        inline void PrintRegistry()
        {
            for (const auto& record : m_AssetsRegistry)
            {
                Debug::Message("ID: {}; Path: {}.", record.AssetID, record.Path.string());
            }
        }
    INTERNAL:
        AssetManager() = default;
        ~AssetManager() override = default;

        int32_t Startup(const ProjectSettings& settings) override;
        int32_t Shutdown() override;

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

        uid_t AssignID(RigelAsset* ptr);

        std::atomic<uid_t> m_NextID = 1;

        mutable std::shared_mutex m_RegistryMutex;
        std::mutex m_IDMutex;

        plf::colony<AssetRegistryRecord> m_AssetsRegistry;
    };
}
