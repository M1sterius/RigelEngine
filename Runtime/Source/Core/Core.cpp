#include "Core.hpp"
#include "InternalUtils.hpp"

namespace rge
{
    Core* Core::m_Instance = nullptr;

    void Core::Startup()
    {
        ASSERT(m_Instance == nullptr, "Core subsystem has already been initialized!")
        m_Instance = new Core();
    }

    void Core::Shutdown()
    {
        ASSERT(m_Instance != nullptr, "Unable to shutdown core subsystem, "
              "it is either not initialized yet or has already been shut down!")

        delete m_Instance;
        m_Instance = nullptr;
    }

    void Core::Print() const
    {
        printf("Core is working!\n");
    }
}
