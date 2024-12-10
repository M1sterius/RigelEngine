#include "Filesystem.hpp"
#include "RGE_PCH.hpp"
#include "STD_PCH.hpp"

namespace rge
{
    Filesystem* Filesystem::m_Instance = nullptr;

    void Filesystem::Startup()
    {
        ASSERT(m_Instance == nullptr, "Filesystem subsystem has already been initialized!")
        m_Instance = new Filesystem();

        m_Instance->m_WorkingDirectory = std::filesystem::current_path();
    }

    void Filesystem::Shutdown()
    {
        ASSERT(m_Instance != nullptr, "Unable to shutdown Filesystem subsystem because "
              "it's either not initialized yet or has already been shut down!")

        delete m_Instance;
        m_Instance = nullptr;


    }
}
