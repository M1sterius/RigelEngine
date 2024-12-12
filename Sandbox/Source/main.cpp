#include "RigelEngine.hpp"

int32 main(int32 argc, char* argv[])
{
    rge::Core::Startup();
    auto core = rge::Core::Get();

    while (core->IsRunning())
    {
        printf("%f\n", rge::Time::GetDeltaTime());
        core->EngineUpdate();
    }

    rge::Core::Shutdown();
}
