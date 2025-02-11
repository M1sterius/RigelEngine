#pragma once

/*
 * Rigel engine API master header
 */

#ifdef RIGEL_ENGINE

#include "Engine.hpp"

// Subsystems
#include "EventManager.hpp"
#include "SceneManager.hpp"
#include "Renderer.hpp"

// ECS
#include "RigelHandle.hpp"
#include "SceneHandle.hpp"
#include "GOHandle.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"
#include "Component.hpp"

#include "Components/Transform.hpp"

// Misc
#include "Time.hpp"
#include "Debug.hpp"
#include "Core.hpp"
#include "Core.hpp"
#include "ISerializable.hpp"
#include "GLM_Serializer.hpp"
#include "File.hpp"

#include "json.hpp"
#include "Math.hpp"

#endif
