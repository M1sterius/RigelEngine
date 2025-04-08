#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"
#include "ISerializable.hpp"
#include "ComponentHandle.hpp"
#include "GOHandle.hpp"
#include "SceneHandle.hpp"

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

        NODISCARD inline SceneHandle GetScene() const { return m_Scene; }
        NODISCARD inline GOHandle GetGameObject() const { return m_GameObject; }
    protected:
        Component();

        virtual void OnLoad() { }
        virtual void OnStart() { }
        virtual void OnDestroy() { }

        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        Engine& Engine;
        AssetManager& AssetManager;
        EventManager& EventManager;
        SceneManager& SceneManager;
        WindowManager& WindowManager;
    private:
        SceneHandle m_Scene;
        GOHandle m_GameObject;

        friend class GameObject;
    };
}
