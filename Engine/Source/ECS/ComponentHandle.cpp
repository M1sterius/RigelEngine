#include "ComponentHandle.hpp"
#include "Engine.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "SceneHandle.hpp"
#include "GOHandle.hpp"
#include "GameObject.hpp"
#include "Component.hpp"

namespace rge::backend
{
    bool ValidateComponentHandleImpl(const uid_t cmpID, const uid_t goID, const uid_t sceneID)
    {
        const auto& sceneManager = Engine::Get().GetSceneManager();

        const auto scene = sceneManager.GetSceneByID(sceneID);
        if (scene.IsNull())
            return false;

        const auto go = scene->GetGameObjectByID(goID);
        if (go.IsNull())
            return false;

        const auto cmp = go->GetComponentByID(cmpID);
        if (cmp.IsNull())
            return false;

        return true;
    }
}
