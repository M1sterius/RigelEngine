#pragma once

#include "Core.hpp"
#include "ISerializable.hpp"
#include "RigelObject.hpp"

#define DEFINE_COMPONENT_TYPE(Type) NODISCARD const char* GetTypeName() const override { return #Type; }

namespace rge
{
    class Component : public RigelObject, public ISerializable
    {
    public:
        NODISCARD virtual const char* GetTypeName() const = 0;
    protected:
        Component();
        ~Component() override = default;

        NODISCARD nlohmann::json Serialize() const override;
        void Deserialize(const nlohmann::json& json) override;
    private:
        friend class GameObject;
    };
}
