#include "RigelEngine.hpp"
#include "TestComponent.hpp"

#include <iostream>

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene("SampleScene");
    // const auto json = rge::File::ReadJSON("Assets/Scenes/Scene.rigelscene");
    // scene->Deserialize(json);

    auto go = scene->InstantiateGO();
    go->AddComponent<rge::Transform>();
    go->AddComponent<TestComponent>();

    auto go1 = scene->InstantiateGO();
    go1->AddComponent<rge::Transform>();

    auto go2 = scene->InstantiateGO();
    go2->AddComponent<rge::Transform>();
    go2->AddComponent<TestComponent>();

    auto go3 = scene->InstantiateGO();
    go3->AddComponent<rge::Transform>();

    auto go4 = scene->InstantiateGO();
    go4->AddComponent<rge::Transform>();
    //
    // rge::File::WriteJSON("Assets/Scenes/Scene.rigelscene", scene->Serialize().dump());

    sceneManager.LoadScene(scene);

    engine->Run();
}
