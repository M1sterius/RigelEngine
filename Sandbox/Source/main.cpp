#include "RigelEngine.hpp"

#include <iostream>

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene();
    auto go = scene->AddGameObject();
    go->AddComponent<rge::Transform>();
    auto t = go->GetComponent<rge::Transform>();
    t->DoStuff();

    sceneManager.LoadScene(scene);

    engine->Run();
}
