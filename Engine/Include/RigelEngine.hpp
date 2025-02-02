#pragma once

/*
 * Rigel engine API master header
 *
 *
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
#include "EngineTools/Time.hpp"
#include "EngineTools/Debug.hpp"
#include "Utils/HeaderUtils/RGE_API.hpp"
#include "Utils/Serialization/ISerializable.hpp"
#include "Utils/Serialization/GLM_Serializer.hpp"
#include "json.hpp"

#endif
