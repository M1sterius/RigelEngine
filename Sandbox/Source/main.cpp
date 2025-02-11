#include "RigelEngine.hpp"

#include <iostream>

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene();

    auto go = scene->InstantiateGO();
    scene->InstantiateGO();
    scene->InstantiateGO();
    scene->InstantiateGO();

    scene->DestroyGO(go);

    const auto json = scene->Serialize();
    rge::File::WriteText("scene.json", json.dump());

    sceneManager.LoadScene(scene);

    engine->Run();
}
