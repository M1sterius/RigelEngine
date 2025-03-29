#define RGE_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"

class Game
{
public:
    explicit Game(rge::Engine& engine)
        : m_Engine(engine)
    {

    }
    ~Game() = default;

    void Init() const
    {
        auto& sceneManager = m_Engine.GetSceneManager();
        auto& assetManager = m_Engine.GetAssetManager();

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

    void Run() const
    {
        m_Engine.Run();
    }

    NODISCARD rge::SceneHandle MakeDefaultScene() const
    {
        auto& sceneManager = m_Engine.GetSceneManager();
        auto scene = sceneManager.CreateScene("Default scene");

        auto go0 = scene->Instantiate();
        go0->AddComponent<rge::Transform>();
        go0->AddComponent<rge::Camera>(1, 1, 1);

        for (int i = 0; i < 10000; ++i)
        {
            auto go = scene->Instantiate();
            go->AddComponent<rge::Transform>();

            if (i == 69)
            {
                scene->Destroy(go);
            }
        }

        return scene;
    }
private:
    rge::Engine& m_Engine;
};

int32_t main(const int32_t argc, char** argv)
{
    const auto engine = rge::Engine::CreateInstance(argc, argv);
    rge::Time::SetTargetFPS(30);

    const auto game = Game(*engine);
    game.Init();
    game.Run();
}
