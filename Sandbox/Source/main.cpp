#include "RigelEngine.hpp"

int32 main(int32 argc, char* argv[])
{
    rge::Core::Startup();
    auto core = rge::Core::Get();

    printf(core->GetFilesystem()->GetWorkingDirectory().string().c_str());

    while (core->IsRunning())
    {
        core->EngineUpdate();
        break; // temp
    }

    rge::Core::Shutdown();
}
