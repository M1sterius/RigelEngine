#pragma once

/*
 * Rigel engine API master header
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
#include "ISerializable.hpp"
#include "GLM_Serializer.hpp"
#include "File.hpp"
#include "Directory.hpp"
#include "KeysAndButtons.hpp"
#include "Math.hpp"
#include "json.hpp"

#endif
