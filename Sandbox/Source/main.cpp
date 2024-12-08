#include "RigelEngine.hpp"

int32 main(int32 argc, char* argv[])
{
    rge::Core::Startup();
    auto core = rge::Core::Get();

    while (core->IsRunning())
    {
        core->EngineUpdate();
        break; // temp
    }

    rge::Core::Shutdown();
}
