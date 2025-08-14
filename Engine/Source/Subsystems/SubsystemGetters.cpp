#include "Subsystems/SubsystemGetters.hpp"
#include "Engine.hpp"

namespace Rigel
{
    Ref<Engine> GetEngine()
    {
        return Engine::Get();
    }

    Ref<Time> GetTime()
    {
        return GetEngine()->GetTime();
    }

    Ref<AssetManager> GetAssetManager()
    {
        return GetEngine()->GetAssetManager();
    }

    Ref<SceneManager> GetSceneManager()
    {
        return GetEngine()->GetSceneManager();
    }

    Ref<Renderer> GetRenderer()
    {
        return GetEngine()->GetRenderer();
    }

    Ref<EventManager> GetEventManager()
    {
        return GetEngine()->GetEventManager();
    }

    Ref<WindowManager> GetWindowManager()
    {
        return GetEngine()->GetWindowManager();
    }

    Ref<InputManager> GetInputManager()
    {
        return GetEngine()->GetInputManager();
    }

    Ref<PhysicsEngine> GetPhysicsEngine()
    {
        return GetEngine()->GetPhysicsEngine();
    }
}
