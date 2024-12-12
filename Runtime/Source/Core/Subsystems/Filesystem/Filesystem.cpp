#include "Filesystem.hpp"
#include "RGE_PCH.hpp"
#include "STD_PCH.hpp"

namespace rge
{

    const std::filesystem::path& Filesystem::GetWorkingDirectory() const
    {
        return m_WorkingDirectory;
    }

    void Filesystem::Startup()
    {
        m_WorkingDirectory = std::filesystem::current_path();
    }

    void Filesystem::Shutdown()
    {

    }
}
