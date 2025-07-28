#pragma once

#include "Core.hpp"

namespace Rigel
{
    class Engine;
    class Time;
    class AssetManager;
    class SceneManager;
    class Renderer;
    class EventManager;
    class WindowManager;
    class InputManager;
    class PhysicsEngine;

    NODISCARD Ref<Engine> GetEngine();
    NODISCARD Ref<Time> GetTime();
    NODISCARD Ref<AssetManager> GetAssetManager();
    NODISCARD Ref<SceneManager> GetSceneManager();
    NODISCARD Ref<Renderer> GetRenderer();
    NODISCARD Ref<EventManager> GetEventManager();
    NODISCARD Ref<WindowManager> GetWindowManager();
    NODISCARD Ref<InputManager> GetInputManager();
    NODISCARD Ref<PhysicsEngine> GetPhysicsEngine();
}
