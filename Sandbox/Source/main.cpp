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
    auto t = go->AddComponent<rge::Transform>(glm::vec3(0.0f), glm::identity<glm::quat>(), glm::vec3(0.0f));
    t->SetPosition(glm::vec3(100, 200, 300));

    auto json = t->Serialize();

    auto go1 = scene->AddGameObject();
    auto t1 = go->AddComponent<rge::Transform>();
    t1->Deserialize(json);

    print(glm::to_string(t1->GetPosition()));

    sceneManager.LoadScene(scene);

    engine->Run();
}
