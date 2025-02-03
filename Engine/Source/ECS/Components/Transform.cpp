#include "Components/Transform.hpp"

#include <iostream>

namespace rge
{
    Transform::Transform() : Component(),
        m_Position(glm::vec3(0.0f)), m_Rotation(glm::identity<glm::quat>()), m_Scale(glm::vec3(1.0f))
    {

    }

    Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) : Component(),
        m_Position(position), m_Rotation(rotation), m_Scale(scale)
    {

    }

    void Transform::DoStuff()
    {
        std::cout << "Stuff\n";
    }


}