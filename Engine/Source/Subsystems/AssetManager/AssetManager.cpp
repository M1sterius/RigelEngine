#include "AssetManager.hpp"
#include "Engine.hpp"
#include "Debug.hpp"
#include "Directory.hpp"
#include "Shader.hpp"
#include "Hash.hpp"
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
        auto vec = std::vector<uid_t>();

        {
            std::unique_lock lock(m_RegistryMutex);
            vec.reserve(m_AssetsRegistry.size());

            for (const auto& record : m_AssetsRegistry)
                vec.emplace_back(record.AssetID);
        }

        for (const auto id : vec)
            this->UnloadImpl(id);

        m_ThreadPool->WaitForAll();
    }

    void AssetManager::UnloadImpl(const uid_t assetID)
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

        // this seems to have fixed an error with empty paths
        if (!assetPtr || path.empty())
            return;

        // just in case the user wants to delete the asset before it's been fully loaded
        assetPtr->WaitReady();

        if (m_EnableAssetLifetimeLogging)
            Debug::Trace("AssetManager::Destroying an asset at path: {}. ", path.string());

        HandleValidator::RemoveHandle<HandleType::AssetHandle>(assetID);
        assetPtr.reset(); // explicitly delete the object just for clarity
    }

    void AssetManager::AssignAssetID(RigelAsset* ptr, const uid_t id)
    {
        using namespace Backend::HandleValidation;

        ptr->OverrideID(id);
        HandleValidator::AddHandle<HandleType::AssetHandle>(id);
    }
}
