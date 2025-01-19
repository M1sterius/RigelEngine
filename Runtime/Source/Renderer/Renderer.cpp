#include "Renderer/Renderer.hpp"
#include "Logger.hpp"

namespace rge
{
    void Renderer::Startup()
    {
        Logger::VerboseMessage("Renderer successfully initialized.");
    }

    void Renderer::Shutdown()
    {
        Logger::VerboseMessage("Renderer successfully shutdown.");
    }
}
