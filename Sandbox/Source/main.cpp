#define RIGEL_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"
#include "TestComponent.hpp"

int32_t main(int32_t argc, char** argv)
{
    auto settings = Rigel::ProjectSettings();
    settings.argc = argc;
    settings.argv = argv;
    settings.GameName = "Sandbox";
    settings.GameVersion = RIGEL_MAKE_VERSION(0, 0, 1);
    settings.TargetFPS = 165;
    settings.WindowTitle = "Sandbox";
    settings.WindowSize = glm::vec2(1920, 1080);
    settings.AssetManagerThreadPoolSize = 4;

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
    camera->GetTransform()->SetLocalPosition({0.0, 1.0, 1.0});

    auto model = scene->Instantiate("Model");
    model->GetTransform()->SetLocalPosition({0, -1.0, 0});
    model->AddComponent<Rigel::ModelRenderer>("Assets/Models/Sponza/Sponza.gltf");
    model->AddComponent<TestComponent>();

    sceneManager.LoadScene(scene);

    engine->Run();
    engine->Shutdown();
}
