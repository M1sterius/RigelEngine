#pragma once

/*
 * Rigel engine API master header
 */

#ifdef RIGEL_ENGINE

#include "Engine.hpp"

// Subsystems
#include "Event.hpp"
#include "EngineEvents.hpp"
#include "EventManager.hpp"
#include "SceneManager.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"
#include "AssetManager.hpp"

// ECS
#include "RigelHandle.hpp"
#include "SceneHandle.hpp"
#include "GOHandle.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"
#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"

#include "Components/Transform.hpp"

// Misc
#include "Time.hpp"
#include "Debug.hpp"
#include "Core.hpp"
#include "Input.hpp"
#include "ISerializable.hpp"
#include "GLM_Serializer.hpp"
#include "File.hpp"
#include "Directory.hpp"
#include "KeysAndButtons.hpp"

#include "json.hpp"
#include "Math.hpp"

#endif
