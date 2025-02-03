#pragma once

#include "HeaderUtils.hpp"
#include "Component.hpp"
#include "RigelHandle.hpp"
#include "glm.hpp"
#include "gtx/quaternion.hpp"

namespace rge
{
    class Transform final : public Component
    {
    public:
        void DoStuff();
    private:
        Transform();
        Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

        glm::vec3 m_Position;
        glm::quat m_Rotation;
        glm::vec3 m_Scale;

        friend class GameObject;
    };
}
