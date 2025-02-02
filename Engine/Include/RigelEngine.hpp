#pragma once

/*
 * Rigel engine API master header
 *
 *
 */

#ifdef RIGEL_ENGINE

#include "Engine.hpp"

// Precompiled headers
#include "Utils/RGE_PCH.hpp"
#include "Utils/STD_PCH.hpp"

// Subsystems
#include "Subsystems/SceneManager.hpp"
#include "Subsystems/Renderer.hpp"

// Object management
#include "ECS/RigelObject.hpp"
#include "ECS/RigelHandle.hpp"

// ECS
#include "ECS/Scene.hpp"
#include "ECS/GameObject.hpp"
#include "ECS/Component.hpp"

// Misc
#include "EngineTools/Time.hpp"
#include "EngineTools/Debug.hpp"
#include "Utils/HeaderUtils/RGE_API.hpp"

#endif
