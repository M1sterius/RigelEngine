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

    void AssetManager::AssignAssetID(RigelAsset* ptr, const uid_t id)
    {
        using namespace Backend::HandleValidation;

        ptr->OverrideID(id);
        HandleValidator::AddHandle<HandleType::AssetHandle>(id);
    }

    void AssetManager::UnloadAllAssets()
    {
        m_AssetsRegistry.clear();
    }
}
