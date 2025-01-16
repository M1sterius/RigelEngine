#include "Engine.hpp"

#include "Time.hpp"
#include "SceneManager.hpp"
#include "RGE_PCH.hpp"
#include "SleepUtility.hpp"
#include "UIDGenerator.hpp"

namespace rge
{
    Engine* Engine::m_Instance = nullptr;

    void Engine::Startup()
    {
        ASSERT(m_Instance == nullptr, "Core subsystem has already been initialized!")
        m_Instance = new Engine();
        m_Instance->InternalStartup(); // Startup redirected to non-static method to avoid having to use m_Instance->
    }
    void Engine::Shutdown()
    {
        ASSERT(m_Instance != nullptr, "Unable to shutdown Core subsystem because "
                  "it's either not initialized yet or has already been shut down!")

        m_Instance->InternalShutdown(); // Shutdown redirected to non-static method to avoid having to use m_Instance->
        delete m_Instance;
        m_Instance = nullptr;
    }

    void Engine::InternalStartup()
    {
        rge::Logger::VerboseMessage("Engine core successfully initialized.");

        // Start up all subsystems
        m_SceneManager = new SceneManager();
        m_SceneManager->Startup();

        rge::Time::GlobalTimeStopwatch.Start();
        m_IsRunning = true;
    }

    void Engine::InternalShutdown()
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
}
