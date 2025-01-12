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

    auto go = scene->AddGameObject();
    auto go1 = scene->AddGameObject();
    auto go2 = scene->AddGameObject();
    auto go3 = scene->AddGameObject();
    auto go4 = scene->AddGameObject();
    auto go5 = scene->AddGameObject();
    print(go2.IsValid());

    sceneManager.LoadScene(scene);

    engine->Run();

    rge::Engine::Shutdown();
}
