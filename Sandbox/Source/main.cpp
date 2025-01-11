#include "RigelEngine.hpp"

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    rge::Logger::SetConsoleColorsVisibility(true);
    rge::Time::SetTargetFPS(60);

    rge::Engine::Startup();
    auto engine = rge::Engine::Get();

    auto& sceneManager = rge::SceneManager::Get();
    auto scene = sceneManager.CreateScene("New scene");

    sceneManager.LoadScene(scene->GetID());

    engine->Run();

    rge::Engine::Shutdown();
}
