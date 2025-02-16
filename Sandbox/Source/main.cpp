#include "RigelEngine.hpp"

#include <iostream>

#define print(x) std::cout << x << '\n'

void OnUpdate(const rge::Event& event)
{
    const auto& e = static_cast<const rge::GameUpdateEvent&>(event);
    print("Update" + std::to_string(e.dt));
}

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    engine->GetEventManager().Subscribe<rge::GameUpdateEvent>(OnUpdate);

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene("Sample Scene");

    scene->Deserialize(rge::File::ReadJSON("scene.json"));

    engine->Run();
}
