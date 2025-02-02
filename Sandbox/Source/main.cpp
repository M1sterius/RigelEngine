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
    auto engine = rge::Engine::CreateInstance();

    rge::Time::SetTargetFPS(30);

    auto& sceneManager = engine->GetSceneManager();
    auto scene = sceneManager.CreateScene();
    auto go = scene->AddGameObject();
    auto t = go->AddComponent<TestComponent>();

    sceneManager.LoadScene(scene);

    engine->Run();
}
