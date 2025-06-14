#define RIGEL_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"
#include "TestComponent.hpp"

int32_t main(const int32_t argc, char** argv)
{
    auto settings = Rigel::ProjectSettings();
    settings.argc = argc;
    settings.argv = argv;
    settings.GameName = "Sandbox";
    settings.GameVersion = RIGEL_MAKE_VERSION(0, 0, 1);
    settings.TargetFPS = 165;
    settings.WindowTitle = "Sandbox";

    const auto engine = Rigel::Engine::CreateInstance();
    if (const auto errorCode = engine->Startup(settings); errorCode != Rigel::ErrorCode::OK)
    {
        Rigel::Debug::Error("Failed to initialize Rigel engine! Error code: {}.", static_cast<int32_t>(errorCode));
        return 1;
    }

    auto& sceneManager = engine->GetSceneManager();
    auto& assetManager = engine->GetAssetManager();
    auto& windowManager = engine->GetWindowManager();

    auto scene = sceneManager.CreateScene();

    auto camera = scene->Instantiate("Camera");
    camera->AddComponent<Rigel::Camera>(glm::radians(60.0), 0.1, 100.0);

    auto cube = scene->Instantiate("Cube");
    cube->GetTransform()->SetLocalPosition({0, 0, -2.5});
    cube->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cube.obj");
    cube->AddComponent<TestComponent>();

    auto cone1 = scene->Instantiate("Cone1");
    cone1->GetTransform()->SetLocalPosition({-1.0, 0, -1.0});
    cone1->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cone.obj");

    auto cone2 = scene->Instantiate("Sphere");
    cone2->GetTransform()->SetLocalPosition({1.0, 0, -1.0});
    cone2->GetTransform()->SetLocalScale(glm::vec3(0.5));
    cone2->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cone.obj");
    cone2->AddComponent<TestComponent>();

    // auto cone1Transform = cone1->GetTransform();
    // auto cone2Transform = cone2->GetTransform();
    //
    // cube->GetTransform()->AddChild(cone1Transform);
    // cube->GetTransform()->AddChild(cone2Transform);

    const auto json = scene->Serialize();

    auto nScene = sceneManager.CreateScene();
    nScene->Deserialize(json);

    sceneManager.LoadScene(nScene);

    engine->Run();
    engine->Shutdown();
}
