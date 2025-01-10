#include "SceneManager.hpp"
#include "Core.hpp"

namespace rge
{
    SceneManager* SceneManager::m_GlobalInstance = nullptr;

    void SceneManager::Startup()
    {

    }

    void SceneManager::Shutdown()
    {

    }

    void SceneManager::Foo()
    {
        printf("OKAY!");
    }
}
