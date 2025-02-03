#pragma once

#include "HeaderUtils.hpp"
#include "Component.hpp"
#include "RigelHandle.hpp"

namespace rge
{
    class Transform final : public Component
    {
    public:
        void DoStuff();
    private:
        Transform() : Component() { }

        friend class GameObject;
    };
}
