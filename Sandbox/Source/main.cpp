#include "RigelEngine.hpp"

#include <iostream>

#define print(x) std::cout << x << '\n'

void OnUpdate(const rge::GameUpdateEvent& event)
{
    print("Update");
}

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    engine->GetEventManager().Subscribe<rge::GameUpdateEvent>(
            reinterpret_cast<rge::EventManager::EventCallback&>(OnUpdate));

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene("Sample Scene");

    scene->Deserialize(rge::File::ReadJSON("scene.json"));

    engine->Run();
}
