#include "RigelEngine.hpp"

#define print(x) std::cout << x << '\n'

class TestComponent : public rge::Component
{
public:
    TestComponent()
    {

    }
    ~TestComponent() override
    {

    }

    void DoSomething()
    {
        print("something is done!");
    }
};

int32_t main(int32_t argc, char* argv[])
{
    rge::Logger::SetConsoleColorsVisibility(true);
    rge::Time::SetTargetFPS(60);

    rge::Engine::Startup();
    auto engine = rge::Engine::Get();

    auto& sceneManager = engine->GetSceneManager();

    auto scene = sceneManager.CreateScene();
    auto go = scene->AddGameObject();
    go->AddComponent<TestComponent>();

    auto cp = go->GetComponent<TestComponent>();

    sceneManager.LoadScene(scene);

    engine->Run();

    rge::Engine::Shutdown();
}
