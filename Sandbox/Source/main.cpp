#define RIGEL_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"
#include "FreeCamController.hpp"

int32_t main(int32_t argc, char** argv)
{
    auto settings = Rigel::ProjectSettings();
    settings.argc = argc;
    settings.argv = argv;
    settings.GameName = "Sandbox";
    settings.GameVersion = RIGEL_MAKE_VERSION(0, 0, 1);
    settings.TargetFPS = 0;
    settings.WindowTitle = "Sandbox";
    settings.WindowSize = glm::vec2(1600, 900);
    settings.AssetManagerThreadPoolSize = 8;

    const auto engine = Rigel::Engine::CreateInstance();
    if (const auto errorCode = engine->Startup(settings); errorCode != Rigel::ErrorCode::OK)
    {
        Rigel::Debug::Error("Failed to initialize Rigel engine! Error code: {}.", static_cast<int32_t>(errorCode));
        return 1;
    }

    auto sceneManager = engine->GetSceneManager();
    auto assetManager = engine->GetAssetManager();
    auto windowManager = engine->GetWindowManager();

    auto scene = sceneManager->CreateScene();

    auto camera = scene->Instantiate("Camera");
    camera->AddComponent<Rigel::Camera>(glm::radians(60.0), 0.1, 100.0);
    camera->AddComponent<FreeCamController>();
    camera->GetTransform()->SetLocalPosition({0.0, 1.0, 1.0});

    auto model = scene->Instantiate("Model");
    model->GetTransform()->SetLocalPosition({0, -1.0, 0});
    model->GetTransform()->SetLocalScale(glm::vec3(0.007f));
    model->AddComponent<Rigel::ModelRenderer>("Assets/Models/SponzaOBJ/Sponza.obj");

    auto model1 = scene->Instantiate("Model1");
    model1->GetTransform()->SetLocalScale(glm::vec3(0.5f));
    model1->GetTransform()->SetLocalPosition(glm::vec3(0.0, -1.0, 0.0));
    model1->GetTransform()->SetLocalRotation(glm::vec3(0.0, glm::radians(90.0f), 0.0));
    model1->AddComponent<Rigel::ModelRenderer>("Assets/Models/UAZ/UAZ.obj");

    sceneManager->LoadScene(scene);

    engine->Run();
    engine->Shutdown();
}
