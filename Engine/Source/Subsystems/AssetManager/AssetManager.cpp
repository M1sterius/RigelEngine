#include "AssetManager.hpp"
#include "Engine.hpp"
#include "Debug.hpp"
#include "Directory.hpp"
#include "Shader.hpp"
#include "Hash.hpp"
#include "json.hpp"

#include <stdexcept>

namespace fs = std::filesystem;

namespace Rigel
{
    AssetManager::AssetManager() { Startup(); }
    AssetManager::~AssetManager() { Shutdown(); }

    void AssetManager::Startup()
    {
        Debug::Trace("Starting up asset manager.");

        m_WorkingDirectory = Directory::WorkingDirectory();
        m_AssetsDirectory = m_WorkingDirectory.concat("/Assets");

        if (!fs::exists(m_AssetsDirectory))
            throw RigelException("Asset manager initialisation failed! Cannot find root Assets directory in the working directory.");

        m_Initialized = true;
    }

    void AssetManager::Shutdown()
    {
        Debug::Trace("Shutting down asset manager.");
    }

    uid_t AssetManager::GetNextAssetID()
    {
        std::unique_lock lock(m_IDMutex);
        return m_NextAssetID++;
    }

    uid_t AssetManager::AssignID(RigelAsset* ptr)
    {
        const auto ID = GetNextAssetID();
        ptr->OverrideID(ID);
        return ID;
    }

    void AssetManager::UnloadAllAssets()
    {
        m_AssetsRegistry.clear();
    }
}
