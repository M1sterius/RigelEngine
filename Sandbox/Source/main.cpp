#include "RigelEngine.hpp"

#define print(x) std::cout << x << '\n'

class TestComponent : public rge::Component
{
public:
    void Load() override
    {
        print("Load!");
    }

    void Start() override
    {
        print("Start!");
    }

    void Update() override
    {
        printf("Frame: %zu, Delta time: %.4f\n", rge::Time::GetFrameCount(), rge::Time::GetDeltaTime());
    }
};

class Rigidbody : public rge::Component
{
public:
    void Calc()
    {
        print("Calc!");
    }
};

int32_t main(int32_t argc, char* argv[])
{
    rge::Logger::SetConsoleColorsVisibility(true);
    rge::Time::SetTargetFPS(60);

    auto engine = rge::Engine::CreateInstance();
    engine->Startup();

    auto& sceneManager = engine->GetSceneManager();
    auto scene = sceneManager.CreateScene();
    auto go = scene->AddGameObject();
    go->AddComponent<TestComponent>();

    sceneManager.LoadScene(scene);

    engine->Run();

    engine->Shutdown();
}
