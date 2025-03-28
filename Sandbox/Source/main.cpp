#define RGE_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"

#include "TestComponent.hpp"

class Game
{
public:
    Game() = default;
    ~Game() = default;

    void Init()
    {
        m_Engine = rge::Engine::CreateInstance();
        rge::Time::SetTargetFPS(30);

        auto& sceneManager = m_Engine->GetSceneManager();
        auto& assetManager = m_Engine->GetAssetManager();

        auto scene = sceneManager.CreateScene();

        try
        {
            const auto json = rge::File::ReadJSON("Assets/Scenes/SampleScene.rigelscene");
            scene->Deserialize(json);
        }
        catch (const std::exception&)
        {
            rge::Debug::Error("Failed to find the file to load the sample scene from!");
        }

        auto dS = MakeDefaultScene();
        sceneManager.LoadScene(dS);
    }

    void Run()
    {
        m_Engine->Run();
    }

    NODISCARD rge::SceneHandle MakeDefaultScene() const
    {
        auto& sceneManager = m_Engine->GetSceneManager();
        auto scene = sceneManager.CreateScene("Default scene");

        auto go0 = scene->InstantiateGO();
        go0->AddComponent<rge::Transform>();
        go0->AddComponent<rge::Camera>(1, 1, 1);

        for (int i = 0; i < 10000; ++i)
        {
            auto go = scene->InstantiateGO();
            go->AddComponent<rge::Transform>();
        }

        return scene;
    }
private:
    std::unique_ptr<rge::Engine> m_Engine;
};

int32_t main(int32_t argc, char** argv)
{
    Game game;
    game.Init();
    game.Run();
}
