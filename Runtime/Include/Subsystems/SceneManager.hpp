#pragma once

#include "Utils/Types.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace rge
{
    class Scene;

    class SceneManager final
    {
    public:
        SceneManager(const SceneManager&) = delete;
        SceneManager operator = (const SceneManager&) = delete;

        NODISCARD std::shared_ptr<Scene> CreateScene(const std::string& name);
        NODISCARD std::shared_ptr<Scene> GetScene(const uid_t id);

        void LoadScene(const std::shared_ptr<Scene>& scene);
        void LoadScene(const uid_t id);

        NODISCARD inline bool IsSceneLoaded() const { return m_LoadedScene != nullptr; }

        NODISCARD static SceneManager& Get();
    INTERNAL:
        void Startup();
        void Shutdown();

        NODISCARD inline std::shared_ptr<Scene> GetLoadedScene() const { return m_LoadedScene; }
    private:
        SceneManager() = default;
        ~SceneManager() = default;

        std::unordered_map<uid_t, std::shared_ptr<Scene>> m_Scenes;
        std::shared_ptr<Scene> m_LoadedScene;
    };

}
