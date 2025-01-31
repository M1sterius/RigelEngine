#include "Engine.hpp"

#include "SceneManager.hpp"
#include "Renderer/Renderer.hpp"
#include "RGE_PCH.hpp"
#include "SleepUtility.hpp"
#include "Backend/Logger.hpp"

namespace rge
{
    Engine* Engine::m_GlobalInstance = nullptr;

    void Engine::Startup()
    {
        // Instantiate and start up all subsystems
        m_SceneManager = new SceneManager();
        m_SceneManager->Startup();

        m_Renderer = new Renderer();
        m_Renderer->Startup();

        m_Running = true;
        m_GlobalTimeStopwatch.Start();
    }

    void Engine::Shutdown()
    {
        // Shut down all subsystems
        m_Renderer->Shutdown();
        delete m_Renderer;

        m_SceneManager->Shutdown();
        delete m_SceneManager;

        m_GlobalTimeStopwatch.Stop();
    }

    SceneManager& Engine::GetSceneManager() const
    {
        ASSERT(m_SceneManager != nullptr, "Attempted to retrieve a rge::SceneManager instance before it has been initialized.")
        return *m_SceneManager;
    }

    Renderer& Engine::GetRenderer() const
    {
        ASSERT(m_Renderer != nullptr, "Attempted to retrieve a rge::Renderer instance before it has been initialized.");
        return *m_Renderer;
    }

    void Engine::Run()
    {
        auto fpsLimitStopwatch = Stopwatch();
        m_DeltaTimeStopwatch.Start();

        while (IsRunning())
        {
            fpsLimitStopwatch.Start();

            EngineUpdate();

            LimitFPS(fpsLimitStopwatch.Stop().AsSeconds(), TargetFps);

            m_DeltaTime = m_DeltaTimeStopwatch.Restart().AsSeconds();
            m_FrameCounter++;
        }
    }

    void Engine::EngineUpdate()
    {
        // Poll and process events
        // Update input state
        // Game update
        // Transform update
        // Pre-render update
        // Scene render
        // Gizmo render
        // GUI render

        GameUpdate();
    }

    void Engine::GameUpdate() const
    {
        if (GetSceneManager().IsSceneLoaded())
        {
            auto scene = GetSceneManager().GetLoadedScene();
            scene->OnGameUpdate();
        }
    }

    std::unique_ptr<Engine> Engine::CreateInstance()
    {
        const auto instance = new Engine();
        ASSERT(instance != nullptr, "Failed to create RigelEngine instance.");
        m_GlobalInstance = instance;
        return std::unique_ptr<Engine>(instance);
    }
}
