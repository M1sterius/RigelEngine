#include "RigelEngine.hpp"
#include "TestComponent.hpp"

#include <iostream>

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene();
    scene->Deserialize(rge::File::ReadJSON("Assets/Scenes/Sample Scene.json"));

    print(scene->Serialize().dump(4));

    sceneManager.LoadScene(scene);

    engine->Run();
}
