#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"
#include "ISerializable.hpp"

namespace Rigel
{
    class Engine;
    class SceneManager;
    class Renderer;
    class EventManager;
    class AssetManager;
    class WindowManager;

    class Component : public RigelObject, public ISerializable
    {
    public:
        ~Component() override = default;

        // Returns the type of derived component represented as a string
        NODISCARD virtual const char* GetTypeName() const = 0;
    protected:
        Component();

        virtual void OnLoad() { }
        virtual void OnStart() { }
        virtual void OnDestroy() { }

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        // Utility methods and fields
        Engine& Engine;
        AssetManager& AssetManager;
        EventManager& EventManager;
        SceneManager& SceneManager;
        WindowManager& WindowManager;
    private:
        friend class GameObject;
    };
}
