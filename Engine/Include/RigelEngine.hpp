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
#include "HeaderUtils.hpp"
#include "ISerializable.hpp"
#include "GLM_Serializer.hpp"

#include "json.hpp"
#include "glm.hpp"
#include "gtx/string_cast.hpp"

#endif
