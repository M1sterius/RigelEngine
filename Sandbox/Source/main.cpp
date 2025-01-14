#include "RigelEngine.hpp"

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    rge::Logger::SetConsoleColorsVisibility(true);
    rge::Time::SetTargetFPS(60);

    rge::Engine::Startup();
    auto engine = rge::Engine::Get();

    auto& sceneManager = engine->GetSceneManager();
    auto scene = sceneManager.CreateScene();

    sceneManager.LoadScene(scene);

    engine->Run();

    rge::Engine::Shutdown();
}
