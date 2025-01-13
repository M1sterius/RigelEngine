#pragma once

#include "Utils/Types.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace rge
{
    class Scene;
    class SceneHandle;

    class SceneManager final
    {
    public:
        SceneManager(const SceneManager&) = delete;
        SceneManager operator = (const SceneManager&) = delete;
        NODISCARD static SceneManager& Get();

        NODISCARD inline bool IsSceneLoaded() const { return m_LoadedSceneID != NULL_ID; }

        NODISCARD SceneHandle CreateScene(const std::string& name = "New Scene");
        NODISCARD SceneHandle GetSceneByID(const uid_t id);

        void LoadScene(const SceneHandle& scene);
    INTERNAL:
        void Startup();
        void Shutdown();
    private:
        SceneManager() = default;
        ~SceneManager() = default;

        std::unordered_map<uid_t, Scene*> m_Scenes;
        uid_t m_LoadedSceneID = NULL_ID;
    };
}
