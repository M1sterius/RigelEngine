#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Utilities/Filesystem/Directory.hpp"
#include "Assets/Shader.hpp"
#include "Assets/Model.hpp"
#include "Engine.hpp"
#include "Debug.hpp"

#include <ranges>

namespace Rigel
{
    ErrorCode AssetManager::Startup(const ProjectSettings& settings)
    {
        Debug::Trace("Starting up asset manager.");

        m_EnableAssetLifetimeLogging = settings.EnableAssetLifetimeLogging;
        m_ThreadPool = std::make_unique<ThreadPool>(settings.AssetManagerThreadPoolSize);

        Debug::Trace("AssetManager::Created thread pool with {} threads.", m_ThreadPool->GetSize());

        m_Initialized = true;
        return ErrorCode::OK;
    }

    ErrorCode AssetManager::Shutdown()
    {
        Debug::Trace("Shutting down asset manager.");

        return ErrorCode::OK;
    }

    void AssetManager::Unload(const uid_t assetID)
    {
        m_ThreadPool->Enqueue([this, assetID]
        {
            std::unique_ptr<RigelAsset> assetPtr;
            std::filesystem::path path;

            {
                std::unique_lock lock(m_RegistryMutex);

                for (auto& [hash, entry] : m_Registry)
                {
                    if (entry.AssetID == assetID)
                    {
                        assetPtr = std::move(entry.Asset);
                        path = entry.Path;

                        m_Registry.erase(hash);
                        break;
                    }
                }
            }

            // This check prevents nullptr dereference when Unload is called on the same asset ID multiple times
            if (!assetPtr)
                return;

            // just in case the user wants to unload an asset before it's been fully loaded
            assetPtr->WaitReady();

            if (m_EnableAssetLifetimeLogging)
                Debug::Trace("AssetManager::Destroying an asset at path: {}.", path.string());

            assetPtr.reset(); // explicitly delete the object just for clarity
        });
    }

    void AssetManager::UnloadAllAssets()
    {
        // We must make sure that persistent assets get unloaded after all normal assets
        // because some of them use persistent assets as default/fallback values

        auto normalAssets = std::vector<uid_t>();
        auto persistentAssets = std::vector<uid_t>();

        {
            std::unique_lock lock(m_RegistryMutex);

            for (const auto& entry : m_Registry | std::views::values)
            {
                if (entry.Asset->m_IsPersistent)
                    persistentAssets.push_back(entry.AssetID);
                else
                    normalAssets.push_back(entry.AssetID);
            }
        }

        for (const auto id : normalAssets)
            Unload(id);
        m_ThreadPool->WaitForAll();

        for (const auto id : persistentAssets)
            Unload(id);
        m_ThreadPool->WaitForAll();
    }
}
