#include "AssetManager.hpp"
#include "Logger.hpp"
#include "Engine.hpp"
#include "Debug.hpp"

#include <stdexcept>

namespace fs = std::filesystem;

namespace rge
{

    AssetManager::AssetManager() { Startup(); }

    AssetManager::~AssetManager() { Shutdown(); }

    void AssetManager::Startup()
    {
        VERBOSE_MESSAGE("Starting up asset manager.");

        m_WorkingDirectory = Engine::Get().GetWorkingDirectory();
        m_AssetsDirectory = m_WorkingDirectory.concat("/Assets");

        if (!fs::exists(m_AssetsDirectory))
        {
            THROW_RUNTIME_ERROR("Asset manager initialisation failed! Cannot find root Assets directory in the working directory.");
        }
    }

    void AssetManager::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down asset manager.");
    }
}