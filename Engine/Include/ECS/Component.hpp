#pragma once

#include "ISerializable.hpp"
#include "Core.hpp"
#include "RigelHandle.hpp"
#include "RigelObject.hpp"
#include "Scene.hpp"

#define DEFINE_COMPONENT_TYPE(Type) NODISCARD const char* GetTypeName() const override { return #Type; }

namespace rge
{
    class Component : public RigelObject, public ISerializable
    {
    public:
        NODISCARD virtual const char* GetTypeName() const = 0;
    protected:
        ~Component() override = default;
        Component();
    private:
        friend class GameObject;
    };
}
