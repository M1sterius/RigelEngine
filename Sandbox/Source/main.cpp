#include "RigelEngine.hpp"
#include "TestComponent.hpp"

int32_t main(int32_t argc, char* argv[])
{
    const auto engine = rge::Engine::CreateInstance();
    auto& sceneManager = engine->GetSceneManager();

    rge::Time::SetTargetFPS(30);

    auto scene = sceneManager.CreateScene("SampleScene");
    const auto json = rge::File::ReadJSON("Assets/Scenes/Scene.rigelscene");
    scene->Deserialize(json);

    sceneManager.LoadScene(scene);

    engine->Run();
}
