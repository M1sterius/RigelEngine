#pragma once

/*
 * Rigel engine API master header
 */

/*
 * Define any of the settings below before including this header
 *
 * RGE_USE_64_BIT_ID_TYPE - to make uid_t 64-bit wide
 * RGE_ENABLE_HANDLE_VALIDATION - to enable handle validation for each handle access attempt
 */

#ifdef RIGEL_ENGINE

// Core headers
#include "Core.hpp"
#include "Engine.hpp"

// Subsystems
#include "EventManager.hpp"
#include "SceneManager.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"
#include "AssetManager.hpp"
#include "WindowManager.hpp"
#include "PhysicsEngine.hpp"

// Subsystem-related classes
#include "Event.hpp"
#include "EngineEvents.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"
#include "Shader.hpp"
#include "KeysAndButtons.hpp"

// Debug
#include "Debug.hpp"
#include "Exceptions.hpp"

// Tools
#include "Input.hpp"
#include "Time.hpp"

// ECS
#include "RigelHandle.hpp"
#include "SceneHandle.hpp"
#include "GOHandle.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"
#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"

// Components
#include "Transform.hpp"
#include "Camera.hpp"

// Utility
#include "ThreadPool.hpp"
#include "SleepUtility.hpp"
#include "ISerializable.hpp"
#include "GLM_Serializer.hpp"
#include "File.hpp"
#include "Directory.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "json.hpp"

#endif
