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
    go->AddComponent<rge::Transform>(glm::vec3(0.0f), glm::identity<glm::quat>(), glm::vec3(0.0f));

    sceneManager.LoadScene(scene);

    engine->Run();
}
