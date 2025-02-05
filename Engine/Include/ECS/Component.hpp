#pragma once

#include "ISerializable.hpp"
#include "HeaderUtils.hpp"
#include "RigelHandle.hpp"
#include "RigelObject.hpp"
#include "Scene.hpp"

namespace rge
{
    class Component : public RigelObject, public ISerializable
    {
    protected:
        ~Component() override = default;
        Component();
    private:
        friend class GameObject;
    };
}
