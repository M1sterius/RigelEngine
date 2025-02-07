#include "RigelEngine.hpp"

#include <iostream>
#include <functional>

#define print(x) std::cout << x << '\n'

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene();

    auto go1 = scene->AddGameObject();
    scene->AddGameObject();
    scene->AddGameObject();
    auto go = scene->AddGameObject();
    scene->AddGameObject();

    auto t = go->AddComponent<rge::Transform>();

    print(t.GetID());

    // auto go = scene->AddGameObject();
    // auto t = go->AddComponent<rge::Transform>();
    // t->SetPosition(glm::vec3(1, 2, 3));
    //
    // auto json = go->Serialize();
    //
    // auto object = scene->AddGameObject();
    //
    // object->Deserialize(json);
    //
    // auto ts = object->GetComponent<rge::Transform>();
    //
    // print(scene->Serialize().dump(4));
    //
    // sceneManager.LoadScene(scene);

    engine->Run();
}
