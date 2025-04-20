#include "SceneHandle.hpp"
#include "HandleValidator.hpp"

namespace Rigel
{
    SceneHandle::SceneHandle(Scene* ptr, const uid_t id) : RigelHandle(ptr, id) { }

    bool SceneHandle::IsNull() const
    {
        return m_Ptr == nullptr || m_ID == NULL_ID;
    }

    bool SceneHandle::IsValid() const
    {
        using namespace Backend::HandleValidation;
        return HandleValidator::Validate<HandleType::SceneHandle>(m_ID);
    }
}
