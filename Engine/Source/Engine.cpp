#include "Engine.hpp"

#include "SceneManager.hpp"
#include "Subsystems/Renderer.hpp"
#include "RGE_PCH.hpp"
#include "SleepUtility.hpp"
#include "Backend/Logger/Logger.hpp"
#include "json.hpp"

#include "Utils/Serialization/GLM_Serializer.hpp"

namespace rge
{
    Engine* Engine::m_GlobalInstance = nullptr;

    Engine::Engine() { Startup(); }
    Engine::~Engine() { Shutdown(); }

    std::unique_ptr<Engine> Engine::CreateInstance()
    {
        ASSERT(m_GlobalInstance == nullptr, "Only a single instance of RigelEngine core class is allowed!")
        const auto instance = new Engine();
        ASSERT(instance != nullptr, "Failed to create RigelEngine instance!");
        m_GlobalInstance = instance;
        return std::unique_ptr<Engine>(instance);
    }

    void Engine::Startup()
    {
        // Instantiate and start up all subsystems and global tools
        m_Logger = std::make_unique<Logger>();
        m_SceneManager = std::make_unique<SceneManager>();
        m_Renderer = std::make_unique<Renderer>();

        auto vec = glm::vec3(1, 2, 3);
        auto pos = GLM_Serializer::Serialize(vec);
        auto scale = GLM_Serializer::Serialize(glm::vec3(1, 1, 1));

//        std::cout << json.dump(4) << '\n';

        nlohmann::json jM;

        jM["position"] = pos;
        jM["scale"] = scale;

        std::cout << jM.dump(4) << '\n';

        auto vecD = GLM_Serializer::Deserialize(jM["position"]);

        m_Running = true;
        m_GlobalTimeStopwatch.Start();
    }

    void Engine::Shutdown()
    {
        // Shut down all subsystems and global tools
        m_Renderer.reset();
        m_SceneManager.reset();
        m_Logger.reset();

        m_GlobalTimeStopwatch.Stop();
    }

    SceneManager& Engine::GetSceneManager() const
    {
        ASSERT(m_SceneManager, "Attempted to retrieve a rge::SceneManager instance before it has been initialized.")
        return *m_SceneManager;
    }

    Renderer& Engine::GetRenderer() const
    {
        ASSERT(m_Renderer, "Attempted to retrieve a rge::Renderer instance before it has been initialized.");
        return *m_Renderer;
    }

    Logger& Engine::GetLogger() const
    {
        ASSERT(m_Logger, "Attempted to retrieve a rge::Logger instance before it has been initialized.");
        return *m_Logger;
    }

    void Engine::Run()
    {
        auto fpsLimitStopwatch = Stopwatch();
        m_DeltaTimeStopwatch.Start();

        while (IsRunning())
        {
            fpsLimitStopwatch.Start();

            EngineUpdate();

            LimitFPS(fpsLimitStopwatch.Stop().AsSeconds(), TargetFps);

            m_DeltaTime = m_DeltaTimeStopwatch.Restart().AsSeconds();
            m_FrameCounter++;
        }
    }

    void Engine::EngineUpdate()
    {
        // Poll and process events
        // Update input state
        // Game update
        // Transform update
        // Pre-render update
        // Scene render
        // Gizmo render
        // GUI render

        GameUpdate();
    }

    void Engine::GameUpdate() const
    {
        if (GetSceneManager().IsSceneLoaded())
        {
            auto scene = GetSceneManager().GetLoadedScene();
            scene->OnGameUpdate();
        }
    }
}
