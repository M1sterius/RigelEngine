#include "SceneManager.hpp"
#include "Core.hpp"
#include "RGE_PCH.hpp"

namespace rge
{
    SceneManager* SceneManager::m_GlobalInstance = nullptr;

    void SceneManager::Startup()
    {


        rge::Logger::VerboseMessage("SceneManager subsystem successfully initialized.");
    }

    void SceneManager::Shutdown()
    {


        rge::Logger::VerboseMessage("SceneManager subsystem successfully shut down.");
    }

    void SceneManager::Foo()
    {
        printf("OKAY!");
    }
}
