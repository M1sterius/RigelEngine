#include "GOHandle.hpp"
#include "Engine.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"
#include "HandleValidator.hpp"

namespace Rigel
{
    GOHandle::GOHandle(GameObject* ptr, const uid_t id) : RigelHandle(ptr, id) { }

    bool GOHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ID == NULL_ID;
    }

    bool GOHandle::IsValid() const
    {
        using namespace Backend::HandleValidation;
        return HandleValidator::Validate<HandleType::GOHandle>(m_ID);
    }
}