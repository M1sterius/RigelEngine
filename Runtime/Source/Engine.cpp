#include "Engine.hpp"

#include "Time.hpp"
#include "SceneManager.hpp"
#include "RGE_PCH.hpp"
#include "SleepUtility.hpp"
#include "UIDGenerator.hpp"

namespace rge
{
    Engine* Engine::m_GlobalInstance = nullptr;

    void Engine::Startup()
    {
        rge::Logger::VerboseMessage("Engine core successfully initialized.");

        // Start up all subsystems
        m_SceneManager = new SceneManager();
        m_SceneManager->Startup();

        rge::Time::GlobalTimeStopwatch.Start();
        m_IsRunning = true;
    }
    void Engine::Shutdown()
    {
        // Shut down all subsystems
        m_SceneManager->Shutdown();
        delete m_SceneManager;

        rge::Time::GlobalTimeStopwatch.Stop();

        rge::Logger::VerboseMessage("Engine core successfully shut down.");
    }

    SceneManager& Engine::GetSceneManager() const
    {
        ASSERT(m_SceneManager != nullptr, "Attempted to retrieve a SceneManager instance before it has been initialized.")
        return *m_SceneManager;
    }

    void Engine::Run()
    {
        while (IsRunning())
        {
            rge::Time::SetDeltaTime(rge::Time::DeltaTimeStopwatch.Restart().AsSeconds());
            rge::Time::FrameCount++;

            static auto sw = Stopwatch();
            sw.Start();

            EngineUpdate();

            LimitFPS(sw.Stop().AsSeconds(), Time::GetTargetFPS());
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
        auto instance = new Engine();
        ASSERT(instance != nullptr, "Failed to create RigelEngine instance.");
        m_GlobalInstance = instance;
        return std::unique_ptr<Engine>(instance);
    }
}
