#define RIGEL_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"
#include "TestComponent.hpp"

int32_t main(const int32_t argc, char** argv)
{
    const auto engine = Rigel::Engine::CreateInstance(argc, argv);
    Rigel::Time::SetTargetFPS(120);

    auto& sceneManager = engine->GetSceneManager();
    auto& assetManager = engine->GetAssetManager();
    auto& windowManager = engine->GetWindowManager();

    auto scene = sceneManager.CreateScene();

    auto camera = scene->Instantiate("Camera");
    camera->AddComponent<Rigel::Camera>(glm::radians(60.0), 0.1, 100.0);

    auto cube = scene->Instantiate("Model0");
    cube->GetTransform()->SetPosition({0, 0, -2.5});
    cube->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cube.obj");
    cube->AddComponent<TestComponent>();

    auto cone = scene->Instantiate("Model1");
    cone->GetTransform()->SetPosition({-1.0, 0, -1.0});
    cone->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cone.obj");
    cone->AddComponent<TestComponent>();

    auto cubeTransform = cube->GetTransform();
    cone->GetTransform()->SetParent(cubeTransform);

    const auto json = scene->Serialize();
    Rigel::Debug::Message(json.dump(4));

    sceneManager.LoadScene(scene);

    engine->Run();
}
