#pragma once

#include "RigelSubsystem.hpp"
#include "Utils/Types.hpp"
#include "ECS/Scene.hpp"
#include "Utils/RGE_API.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace rge
{
    class Engine;

    class RGE_API SceneManager final : public RigelSubsystem
    {
    public:
        NODISCARD SceneHandle CreateScene(const std::string& name = "New Scene");
        NODISCARD SceneHandle GetSceneByID(const uid_t id) const;

        NODISCARD bool IsSceneLoaded() const { return !m_LoadedScene.IsNull(); }
        NODISCARD bool IsSceneHandleValid(const SceneHandle& handle) const;
        NODISCARD SceneHandle GetLoadedScene() const;

        void LoadScene(SceneHandle& scene);
    INTERNAL:
        void Startup() override;
        void Shutdown() override;

        std::unordered_map<uid_t, Scene*> m_Scenes;
        SceneHandle m_LoadedScene;
    private:
        SceneManager();
        ~SceneManager() = default;

        friend class Engine;
    };
}
