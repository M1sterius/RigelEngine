#include "RigelEngine.hpp"

int32 main(int32 argc, char* argv[])
{
    rge::Core::Startup();
    auto instance = rge::Core::Get();

    while (instance->IsRunning())
    {
        instance->EngineUpdate();
    }

    rge::Core::Shutdown();
}
