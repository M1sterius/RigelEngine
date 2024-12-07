#include "Core.hpp"

namespace rge
{
    Core* Core::m_Instance = nullptr;

    void Core::Startup()
    {
        ASSERT(m_Instance == nullptr, "Core subsystem has already been initialized!")
        m_Instance = new Core();

        m_Instance->m_IsRunning = true;
    }

    void Core::Shutdown()
    {
        ASSERT(m_Instance != nullptr, "Unable to shutdown Core subsystem because "
              "it's either not initialized yet or has already been shut down!")

        delete m_Instance;
        m_Instance = nullptr;
    }

    void Core::EngineUpdate()
    {

    }
}
