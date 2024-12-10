#include "RigelEngine.hpp"

int32 main(int32 argc, char* argv[])
{
    rge::Core::Startup();
    auto core = rge::Core::Get();

    std::cout << core->GetFilesystem()->GetWorkingDirectory().string();

    while (core->IsRunning())
    {
        core->EngineUpdate();
        break; // temp
    }

    rge::Core::Shutdown();
}
