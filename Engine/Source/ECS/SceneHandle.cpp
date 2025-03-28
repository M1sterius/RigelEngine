#include "SceneHandle.hpp"
#include "SceneManager.hpp"
#include "Engine.hpp"

namespace rge
{
    SceneHandle::SceneHandle(Scene* ptr, const uid_t id) : RigelHandle(ptr, id) { }

    bool SceneHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ID == NULL_ID;
    }

    bool SceneHandle::IsValid() const
    {
        const auto& manager = Engine::Get().GetSceneManager();
        return manager.ValidateSceneHandle(*this);
    }
}