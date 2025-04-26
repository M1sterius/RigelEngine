#define RGE_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"
#include "TestComponent.hpp"

void Sleep(const uint32_t ms)
{
    Rigel::SleepUtility::PreciseSleep(Rigel::ElapsedTime::FromMilliseconds(ms));
}

int32_t main(const int32_t argc, char** argv)
{
    // auto pool = Rigel::ThreadPool(4);
    // auto sw = Rigel::Stopwatch::StartNew();
    //
    // auto future = pool.Enqueue(Sleep, 1000);
    // future.get();
    //
    // pool.Enqueue(Sleep, 1000);
    // pool.Enqueue(Sleep, 150);
    //
    // pool.WaitForAll();
    //
    // Rigel::Debug::Message("{}", sw.Stop().AsMilliseconds());
    // std::cin.get();

    const auto engine = Rigel::Engine::CreateInstance(argc, argv);
    Rigel::Time::SetTargetFPS(120);

    auto& sceneManager = engine->GetSceneManager();
    auto& assetManager = engine->GetAssetManager();
    auto& windowManager = engine->GetWindowManager();

    auto scene = sceneManager.CreateScene();

    auto camera = scene->Instantiate("Camera");
    camera->AddComponent<Rigel::Camera>(glm::radians(60.0), 0.1, 100.0);

    auto model = scene->Instantiate("Model1");
    model->GetTransform()->SetPosition({0, 0, -2.5});
    model->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cube.obj");
    model->AddComponent<TestComponent>();

    auto model1 = scene->Instantiate("Model2");
    model1->GetTransform()->SetPosition({-1.0, 0, -1.0});
    model1->AddComponent<Rigel::ModelRenderer>("Assets/EngineAssets/Models/cone.obj");
    model1->AddComponent<TestComponent>();

    const auto json = scene->Serialize();
    sceneManager.DestroyScene(scene);

    auto nScene = sceneManager.CreateScene();
    nScene->Deserialize(json);

    sceneManager.LoadScene(nScene);

    assetManager.PrintIDs();

    engine->Run();
}
