#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/AssetManager/BuiltInAssets.hpp"
#include "Utilities/Filesystem/Directory.hpp"
#include "Assets/Shader.hpp"
#include "Assets/Model.hpp"
#include "Assets/Texture2D.hpp"
#include "Engine.hpp"
#include "Debug.hpp"

#include "json.hpp"

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

    void AssetManager::UnloadAllAssets()
    {
        auto basicAssets = std::vector<uid_t>();
        auto persistentAssets = std::vector<uid_t>();

        {
            std::unique_lock lock(m_RegistryMutex);

            for (const auto& entry : m_AssetsRegistry)
            {
                if (entry.Asset->m_IsPersistent)
                    persistentAssets.push_back(entry.AssetID);
                else
                    basicAssets.push_back(entry.AssetID);
            }
        }

        for (const auto id : basicAssets)
            this->Unload(id);
        m_ThreadPool->WaitForAll();

        for (const auto id : persistentAssets)
            this->Unload(id);
        m_ThreadPool->WaitForAll();
    }

    void AssetManager::UnloadImpl(const uid_t assetID)
    {
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

        // this seems to have fixed an error with empty paths
        if (!assetPtr || path.empty())
            return;

        assetPtr->WaitReady(); // just in case the user wants to delete the asset before it's been fully loaded

        if (m_EnableAssetLifetimeLogging)
            Debug::Trace("AssetManager::Destroying an asset at path: {}. ", path.string());

        assetPtr.reset(); // explicitly delete the object just for clarity
    }
}
