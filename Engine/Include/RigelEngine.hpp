#pragma once

/*
 * Rigel engine API master header
 */

/*
 * Define any of the settings below before including this header
 *
 * RIGEL_USE_64_BIT_ID_TYPE - to make uid_t 64-bit wide
 * RIGEL_ENABLE_HANDLE_VALIDATION - to enable handle validation for each handle access attempt
 */

#ifdef RIGEL_ENGINE

// Core headers
#include "Core.hpp"
#include "Utilities/Math/Math.hpp"
#include "Engine.hpp"
#include "ProjectSettings.hpp"

// Subsystems
#include "Subsystems/Time.hpp"
#include "Subsystems/EventSystem/EventManager.hpp"
#include "Subsystems/SceneManager.hpp"
#include "Subsystems/Renderer/Renderer.hpp"
#include "Subsystems/InputManager/InputManager.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"
#include "Subsystems/WindowManager/WindowManager.hpp"
#include "Subsystems/PhysicsEngine/PhysicsEngine.hpp"
#include "Subsystems/SubsystemGetters.hpp"

// Subsystem-related classes
#include "Subsystems/EventSystem/Event.hpp"
#include "Subsystems/EventSystem/EngineEvents.hpp"

#include "Subsystems/InputManager/KeysAndButtons.hpp"
#include "Subsystems/AssetManager/BuiltInAssets.hpp"

// Assets
#include "Assets/RigelAsset.hpp"
#include "Assets/Shader.hpp"
#include "Assets/Model.hpp"
#include "Assets/Texture.hpp"
#include "Assets/Material.hpp"

// Debug
#include "Debug.hpp"

// Handles
#include "Handles/RigelHandle.hpp"
#include "Handles/SceneHandle.hpp"
#include "Handles/GOHandle.hpp"
#include "Handles/AssetHandle.hpp"

// ECS
#include "ECS/Scene.hpp"
#include "ECS/GameObject.hpp"
#include "ECS/Component.hpp"

// Components
#include "Components/Transform.hpp"
#include "Components/Camera.hpp"
#include "Components/ModelRenderer.hpp"

// Utility
#include "Utilities/Reflection/TypeUtility.hpp"
#include "Utilities/Threading/ThreadPool.hpp"
#include "Utilities/Threading/SleepUtility.hpp"
#include "Utilities/Serialization/ISerializable.hpp"
#include "Utilities/Serialization/Serializer.hpp"
#include "Utilities/Filesystem/File.hpp"
#include "Utilities/Filesystem/Directory.hpp"
#include "Utilities/Math/Math.hpp"
#include "Utilities/Math/Random.hpp"
#include "Utilities/ScopeGuard.hpp"
#include "Utilities/Ref.hpp"
#include "json.hpp"

#endif
