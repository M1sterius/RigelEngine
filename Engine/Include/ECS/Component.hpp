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
        // Returns the name of a derived component represented as a string
        NODISCARD virtual const char* GetTypeName() const = 0;
    protected:
        Component();
        ~Component() override = default;

        virtual void OnLoad() { };
        virtual void OnStart() { };
        virtual void OnDestroy() { };

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;
    private:
        friend class GameObject;
    };
}
