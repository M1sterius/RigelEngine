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

#include "Core.hpp"
#include "Engine.hpp"

// Subsystems
#include "Event.hpp"
#include "EngineEvents.hpp"
#include "EventManager.hpp"
#include "SceneManager.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"
#include "AssetManager.hpp"
#include "WindowManager.hpp"
#include "PhysicsEngine.hpp"

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

// Misc
#include "Time.hpp"
#include "Debug.hpp"
#include "Input.hpp"
#include "Exceptions.hpp"
#include "ISerializable.hpp"
#include "GLM_Serializer.hpp"
#include "File.hpp"
#include "Directory.hpp"
#include "KeysAndButtons.hpp"
#include "Math.hpp"
#include "json.hpp"
#include "RigelAsset.hpp"
#include "AssetHandle.hpp"

#endif
