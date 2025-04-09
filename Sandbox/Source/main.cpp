#define RGE_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"
#include "TestComponent.hpp"

int32_t main(const int32_t argc, char** argv)
{
    const auto engine = Rigel::Engine::CreateInstance(argc, argv);
    Rigel::Time::SetTargetFPS(120);

    auto& sceneManager = engine->GetSceneManager();

    auto scene = sceneManager.CreateScene();

    auto camera = scene->Instantiate();
    camera->AddComponent<Rigel::Camera>(3.141592 / 3, 0.1, 100.0);

    auto model = scene->Instantiate();
    model->GetTransform()->SetPosition({0, 0, -2.5});
    model->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cube.obj");
    model->AddComponent<TestComponent>();

    auto model1 = scene->Instantiate();
    model1->GetTransform()->SetPosition({-1.0, 0, -1.0});
    model1->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cone.obj");
    model1->AddComponent<TestComponent>();

    sceneManager.LoadScene(scene);

    engine->Run();
}
