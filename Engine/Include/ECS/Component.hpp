#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"
#include "ISerializable.hpp"
#include "GOHandle.hpp"
#include "SceneHandle.hpp"

namespace Rigel
{
    class Engine;
    class AssetManager;
    class EventManager;
    class PhysicsEngine;
    class Renderer;
    class InputManager;
    class SceneManager;
    class WindowManager;

    class Component : public RigelObject, public ISerializable
    {
    public:
        ~Component() override;

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

        // Utility
        NODISCARD Engine& GetEngine() const;
        NODISCARD AssetManager& GetAssetManager() const;
        NODISCARD EventManager& GetEventManager() const;
        NODISCARD PhysicsEngine& GetPhysicsEngine() const;
        NODISCARD Renderer& GetRenderer() const;
        NODISCARD InputManager& GetInputManager() const;
        NODISCARD SceneManager& GetSceneManager() const;
        NODISCARD WindowManager& GetWindowManager() const;
    private:
        SceneHandle m_Scene;
        GOHandle m_GameObject;

        friend class GameObject;
    };
}
