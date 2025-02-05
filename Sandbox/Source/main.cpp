#include "RigelEngine.hpp"

#include <iostream>

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene();

    auto go = scene->AddGameObject();
    go->AddComponent<rge::Transform>(glm::vec3(0.0f), glm::identity<glm::quat>(), glm::vec3(0.0f));

    auto go1 = scene->AddGameObject();
    go1->AddComponent<rge::Transform>(glm::vec3(1.0f), glm::identity<glm::quat>(), glm::vec3(0.0f));

    auto go2 = scene->AddGameObject();
    go2->AddComponent<rge::Transform>(glm::vec3(2.0f), glm::identity<glm::quat>(), glm::vec3(0.0f));

    auto go3 = scene->AddGameObject();
    go3->AddComponent<rge::Transform>(glm::vec3(3.0f), glm::identity<glm::quat>(), glm::vec3(0.0f));

    sceneManager.LoadScene(scene);

    engine->Run();
}
