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

        const auto h = assetManager.Find<rge::Shader>("bruh.spv");
        rge::Debug::Message("{}", h.IsNull());

//        AssetHandle<Shader> shader = AssetManager.LoadAsset<Shader>("shader.vert", "shader.frag");

        try
        {
            const auto json = rge::File::ReadJSON("Assets/Scenes/SampleScene.rigelscene");
            scene->Deserialize(json);
        }
        catch (const std::exception&)
        {
            rge::Debug::Error("Failed to find the file to load the sample scene from!");
        }

        sceneManager.LoadScene(scene);
    }

    void Run()
    {
        m_Engine->Run();
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
