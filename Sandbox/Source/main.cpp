#define RGE_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"
#include "TestComponent.hpp"

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
        auto [SceneManager,
            Renderer,
            EventManager,
            AssetManager,
            WindowManager,
            InputManager,
            PhysicsEngine] = m_Engine.GetAllSubsystems();

        auto scene = SceneManager.CreateScene();

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
        SceneManager.LoadScene(dS);
    }

    void Run() const
    {
        m_Engine.Run();
    }

    NODISCARD rge::SceneHandle MakeDefaultScene() const
    {
        auto& sceneManager = m_Engine.GetSceneManager();
        auto scene = sceneManager.CreateScene("Default scene");

        auto camera = scene->Instantiate();
        camera->AddComponent<rge::Transform>();
        camera->AddComponent<rge::Camera>(3.141592 / 3, 0.1, 100.0);

        for (size_t i = 0; i < 1000; i++)
        {
            auto go = scene->Instantiate();
            go->AddComponent<rge::Transform>();
            go->AddComponent<rge::ModelRenderer>();
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
