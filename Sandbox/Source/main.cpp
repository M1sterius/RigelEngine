#include "RigelEngine.hpp"

#include <iostream>

#define print(x) std::cout << x << '\n'

#define NAME(n) "somename##n"

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene();

    auto go = scene->AddGameObject();
    go->AddComponent<rge::Transform>(glm::vec3(10.0f), glm::identity<glm::quat>(), glm::vec3(1.0f));

    auto json = go->Serialize();
    print(json.dump(4));

    sceneManager.LoadScene(scene);

    engine->Run();
}
