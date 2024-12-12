#include "Core.hpp"

#include <Time.hpp>
#include "Filesystem.hpp"
#include "RGE_PCH.hpp"

#include <thread>
#include <chrono>

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
        m_Filesystem = new rge::Filesystem();
        m_Filesystem->Startup();
    }

    void Core::InternalShutdown()
    {
        rge::Time::GlobalTimeStopwatch.Stop();

        // Shut down all subsystems
        m_Filesystem->Shutdown();
        delete m_Filesystem;
    }

    /**
     * @brief The main game loop function
     */
    void Core::EngineUpdate()
    {
        rge::Time::DeltaTime = rge::Time::DeltaTimeStopwatch.Restart().AsSeconds();
    }
}
