#define RGE_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"

int32_t main(const int32_t argc, char** argv)
{
    const auto engine = rge::Engine::CreateInstance(argc, argv);
    rge::Time::SetTargetFPS(165);

    auto [SceneManager,
    Renderer,
    EventManager,
    AssetManager,
    WindowManager,
    InputManager,
    PhysicsEngine] = engine->GetAllSubsystems();

    auto scene = SceneManager.CreateScene();

    auto camera = scene->Instantiate();
    camera->AddComponent<rge::Transform>();
    camera->AddComponent<rge::Camera>(3.141592 / 3, 0.1, 100.0);

    auto model = scene->Instantiate();
    model->AddComponent<rge::Transform>();
    model->AddComponent<rge::ModelRenderer>("Assets/EngineAssets/Models/cube.obj");

    SceneManager.LoadScene(scene);

    engine->Run();
}
