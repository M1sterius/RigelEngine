#pragma once

/*
 * Rigel engine API master header
 */

#ifdef RIGEL_ENGINE

#include "Engine.hpp"

// Subsystems
#include "SceneManager.hpp"
#include "Renderer.hpp"

// ECS
#include "RigelHandle.hpp"
#include "Scene.hpp"

// Misc
#include "Time.hpp"
#include "Debug.hpp"
#include "HeaderUtils.hpp"
#include "ISerializable.hpp"
#include "GLM_Serializer.hpp"

#include "json.hpp"

#endif
