#include "RigelEngine.hpp"

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    rge::Core::Startup();
    auto core = rge::Core::Get();

    core->Run();

    rge::Core::Shutdown();
}
