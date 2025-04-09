#define RGE_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"
#include "TestComponent.hpp"

int32_t main(const int32_t argc, char** argv)
{
    const auto engine = Rigel::Engine::CreateInstance(argc, argv);
    Rigel::Time::SetTargetFPS(120);

    constexpr float32_t PI = 3.141592;

    auto [SceneManager,
    Renderer,
    EventManager,
    AssetManager,
    WindowManager,
    InputManager,
    PhysicsEngine] = engine->GetAllSubsystems();

    auto scene = SceneManager.CreateScene();

    auto camera = scene->Instantiate();
    auto cameraCmp = camera->AddComponent<Rigel::Camera>(3.141592 / 3, 0.1, 100.0);

    auto model = scene->Instantiate();
    model->GetTransform()->SetPosition({0, 0, -2.0});
    model->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cube.obj");
    model->AddComponent<TestComponent>();

    SceneManager.LoadScene(scene);

    engine->Run();
}
