#include "Core.hpp"

#include "Time.hpp"
#include "SceneManager.hpp"
#include "RGE_PCH.hpp"

namespace rge
{
    Core* Core::m_Instance = nullptr;

    void Core::Startup()
    {
        ASSERT(m_Instance == nullptr, "Core subsystem has already been initialized!")
        m_Instance = new Core();
        m_Instance->InternalStartup(); // Startup redirected to non-static method to avoid having to use m_Instance->
    }
    void Core::Shutdown()
    {
        ASSERT(m_Instance != nullptr, "Unable to shutdown Core subsystem because "
                  "it's either not initialized yet or has already been shut down!")

        m_Instance->InternalShutdown(); // Shutdown redirected to non-static method to avoid having to use m_Instance->
        delete m_Instance;
        m_Instance = nullptr;
    }

    void Core::InternalStartup()
    {
        // Start up all subsystems
        m_SceneManager = new SceneManager();
        m_SceneManager->Startup();
        SceneManager::AssignGlobalInstance(m_SceneManager);

        rge::Time::GlobalTimeStopwatch.Start();
        m_IsRunning = true;
    }

    void Core::InternalShutdown()
    {
        // Shut down all subsystems
        m_SceneManager->Shutdown();
        delete m_SceneManager;

        rge::Time::GlobalTimeStopwatch.Stop();
    }

    void Core::Run()
    {
        while (IsRunning())
        {
            EngineUpdate();
        }
    }

    void Core::EngineUpdate()
    {
        rge::Time::DeltaTime = rge::Time::DeltaTimeStopwatch.Restart().AsSeconds();

        // Poll and process events
        // Update input state
        // Game update
        // Transform update
        // Pre-render calculations
        // Scene render
        // Gizmo render
        // GUI render
        // Sleep to target FPS


    }
}
