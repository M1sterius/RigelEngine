#include "RigelEngine.hpp"

#include <iostream>

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene();

    auto go0 = scene->AddGameObject();
    go0->AddComponent<rge::Transform>();

    auto go1 = scene->AddGameObject();
    go1->AddComponent<rge::Transform>();

    auto go2 = scene->AddGameObject();
    go2->AddComponent<rge::Transform>();

    auto go3 = scene->AddGameObject();
    go3->AddComponent<rge::Transform>();

    auto json = scene->Serialize();
    print(json.dump(4));

    engine->Run();
}
