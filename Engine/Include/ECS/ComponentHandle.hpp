#pragma once

#include "RigelHandle.hpp"

namespace rge
{
    namespace backend
    {
        NODISCARD bool ValidateComponentHandleImpl(const uid_t cmpID, const uid_t goID, const uid_t sceneID);
    }

    template<typename T>
    class ComponentHandle final : public RigelHandle<T>
    {
    public:
        ComponentHandle(T* ptr, const uid_t id, const uid_t objectID, const uid_t sceneID)
                :   RigelHandle<T>(ptr, id), m_GameObjectID(objectID), m_SceneID(sceneID) { }

        NODISCARD bool IsNull() const override
        {
            return this->m_Ptr == nullptr || this->m_ID == NULL_ID || m_GameObjectID == NULL_ID || m_SceneID == NULL_ID;
        }

        // Note: This validation comes with really high performance penalty
        // It is recommended to enable handle validation only in debug builds
        NODISCARD bool IsValid() const override
        {
            return backend::ValidateComponentHandleImpl(this->GetID(), m_GameObjectID, m_SceneID);
        }
    private:
        uid_t m_GameObjectID = NULL_ID;
        uid_t m_SceneID = NULL_ID;
    };
}