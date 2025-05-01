#define RIGEL_ENABLE_HANDLE_VALIDATION
#include "RigelEngine.hpp"

class ExampleComponent final : public Rigel::Component
{
public:
    // Every single component MUST use this macro, namespaces are currently not supported
    RIGEL_REGISTER_COMPONENT(ExampleComponent);

    NODISCARD nlohmann::json Serialize() const override
    {
        return Component::Serialize(); // Base method MUST be explicitly called
    }

    bool Deserialize(const nlohmann::json& json) override
    {
        return Component::Deserialize(json); // Base method MUST be explicitly called
    }
private:
    // Every component MUST define default constructor and destructor
    ExampleComponent() = default;
    ~ExampleComponent() override = default;

    // OnStart is called after OnLoad and before the first frame
    void OnStart() override
    {
        // Use this method to subscribe the component to engine events
        SubscribeEvent<Rigel::GameUpdateEvent>(OnGameUpdate);
    }

    // Will be called every frame
    void OnGameUpdate()
    {
        if (Rigel::Input::GetKeyDown(Rigel::KeyCode::SPACE))
        {
            Rigel::Debug::Message("Printed from ExampleComponent!");
        }
    }
};

int32_t main(const int32_t argc, char** argv)
{
    // Create an engine instance as a std::unique_ptr
    const auto engine = Rigel::Engine::CreateInstance(argc, argv);

    // Set target frame rate
    Rigel::Time::SetTargetFPS(120);

    // Access the scene manager
    auto& sceneManager = engine->GetSceneManager();

    // Create a new empty scene
    auto scene = sceneManager.CreateScene();

    // Load scene data from JSON
    const auto sceneJson = Rigel::File::ReadJSON("Assets/EngineAssets/Scenes/ExampleScene.json");
    scene->Deserialize(sceneJson);

    auto go = scene->Instantiate();
    go->AddComponent<ExampleComponent>();

    // Load the scene
    sceneManager.LoadScene(scene);

    // Enter the game loop
    engine->Run();
}
