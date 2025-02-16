#include "Renderer.hpp"
#include "Logger.hpp"

namespace rge
{
    Renderer::Renderer() { Startup(); }

    Renderer::~Renderer() { Shutdown(); }

    void Renderer::Startup()
    {
        VERBOSE_MESSAGE("Starting up renderer.");
    }

    void Renderer::Shutdown()
    {
        VERBOSE_MESSAGE("Shutting down renderer.");
    }
}
