#include "RigelEngine.hpp"

int32 main(int32 argc, char* argv[])
{
    rge::Core::Startup();
    auto instance = rge::Core::Get();
    printf("%i\n", rge::Core::IsInitialized());
    instance->Print();
    rge::Core::Shutdown();
}
