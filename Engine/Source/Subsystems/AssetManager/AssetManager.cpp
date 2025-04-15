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

    void AssetManager::IncrementRefCount(const uid_t id)
    {
        if (id == NULL_ID) return;

        for (auto& record : m_AssetsRegistry)
        {
            if (record.AssetID == id)
                record.RefCount++;
        }

        Debug::Error("Failed to increment reference count for an asset with ID {}. Asset cannot be found!", id);
    }

    void AssetManager::DecrementRefCount(const uid_t id)
    {
        if (id == NULL_ID) return;

        for (auto& record : m_AssetsRegistry)
        {
            if (record.AssetID == id)
                record.RefCount--;
        }

        Debug::Error("Failed to decrement reference count for an asset with ID {}. Asset cannot be found!", id);
    }

    uint32_t AssetManager::GetRefCount(const uid_t id)
    {
        for (auto& record : m_AssetsRegistry)
        {
            if (record.AssetID == id)
                return record.RefCount;
        }

        return -1;
    }

    void AssetManager::UnloadAllAssets()
    {
        m_AssetsRegistry.clear();
    }
}
