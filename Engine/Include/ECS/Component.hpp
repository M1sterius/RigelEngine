#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"
#include "ISerializable.hpp"

namespace rge
{
    class Component : public RigelObject, public ISerializable
    {
    public:
        ~Component() override = default;

        // Returns the name of a derived component represented as a string
        NODISCARD virtual const char* GetTypeName() const = 0;
    protected:
        Component();

        virtual void OnLoad() { };
        virtual void OnStart() { };
        virtual void OnDestroy() { };

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;
    private:
        friend class GameObject;
    };
}
