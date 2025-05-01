#define RIGEL_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"

int32_t main(const int32_t argc, char** argv)
{
    // Create an engine instance as a std::unique_ptr
    const auto engine = Rigel::Engine::CreateInstance(argc, argv);

    // Set target frame rate
    Rigel::Time::SetTargetFPS(120);

    // Access the scene manager
    auto& sceneManager = engine->GetSceneManager();

    // Create a new empty scene
    auto scene = sceneManager.CreateScene();

    // Load scene data from JSON
    const auto sceneJson = Rigel::File::ReadJSON("Assets/EngineAssets/Scenes/ExampleScene.json");
    scene->Deserialize(sceneJson);

    // Load the scene
    sceneManager.LoadScene(scene);

    // Enter the game loop
    engine->Run();
}
