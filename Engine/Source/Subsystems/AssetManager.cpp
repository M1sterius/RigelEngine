#include "AssetManager.hpp"
#include "Logger.hpp"

namespace rge
{

    AssetManager::AssetManager() { Startup(); }

    AssetManager::~AssetManager() { Shutdown(); }

    void AssetManager::Startup()
    {
        VERBOSE_MESSAGE("Starting up asset manager.");
    }

    void AssetManager::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down asset manager.");
    }
}