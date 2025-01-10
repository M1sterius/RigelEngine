#include "Core.hpp"

#include "Time.hpp"
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
        m_IsRunning = true;
        rge::Time::GlobalTimeStopwatch.Start();

        // Start up all subsystems
    }

    void Core::InternalShutdown()
    {
        rge::Time::GlobalTimeStopwatch.Stop();
    }

    void Core::Run()
    {
        while (!IsRunning())
        {
            EngineUpdate();
        }
    }

    /**
     * @brief The main game loop function
     */
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
