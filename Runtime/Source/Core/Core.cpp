#include "Core.hpp"
#include "Filesystem.hpp"
#include "RGE_PCH.hpp"

namespace rge
{
    Core* Core::m_Instance = nullptr;

    void Core::Startup()
    {
        ASSERT(m_Instance == nullptr, "Core subsystem has already been initialized!")
        m_Instance = new Core();
        m_Instance->m_IsRunning = true;

        // Start up all subsystems
        rge::Filesystem::Startup();
        m_Instance->m_Filesystem = rge::Filesystem::Get();
    }

    void Core::Shutdown()
    {
        ASSERT(m_Instance != nullptr, "Unable to shutdown Core subsystem because "
              "it's either not initialized yet or has already been shut down!")

        // Shut down all subsystems
        rge::Filesystem::Shutdown();

        delete m_Instance;
        m_Instance = nullptr;
    }

    /**
     * @brief The main game loop function
     */
    void Core::EngineUpdate()
    {

    }
}
