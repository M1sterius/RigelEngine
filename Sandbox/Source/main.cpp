#define RGE_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"

int32_t main(const int32_t argc, char** argv)
{
    const auto engine = Rigel::Engine::CreateInstance(argc, argv);
    Rigel::Time::SetTargetFPS(120);

    auto [SceneManager,
    Renderer,
    EventManager,
    AssetManager,
    WindowManager,
    InputManager,
    PhysicsEngine] = engine->GetAllSubsystems();

    auto scene = SceneManager.CreateScene();

    auto camera = scene->Instantiate();
    auto cmp = camera->AddComponent<Rigel::Camera>(3.141592 / 3, 0.1, 100.0);
    cmp->GetGameObject()->GetTransform()->SetPosition({1, 2, 3});

    auto model = scene->Instantiate();
    model->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cube.obj");

    SceneManager.LoadScene(scene);

    engine->Run();
}
