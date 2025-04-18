#include "Component.hpp"
#include "Debug.hpp"
#include "json.hpp"

#include "Engine.hpp"

namespace Rigel
{
    // The NULL_ID will be overwritten by GameObject::AddComponent method
    Component::Component() : RigelObject(NULL_ID) { }

    nlohmann::json Component::Serialize() const
    {
        auto json = nlohmann::json();
        json["Type"] = GetTypeName();
        json["ID"] = GetID();
        return json;
    }

    bool Component::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("ID"))
        {
            Debug::Error("Failed to serialize Rigel::Component! Some of the required data is not present in the json object.");
            return false;
        }

        this->OverrideID(json["ID"].get<uid_t>());

        return true;
    }

#pragma region SubsystemGetter
    Engine& Component::GetEngine() const
    {
        return Engine::Get();
    }

    AssetManager& Component::GetAssetManager() const
    {
        return Engine::Get().GetAssetManager();
    }

    EventManager& Component::GetEventManager() const
    {
        return Engine::Get().GetEventManager();
    }

    PhysicsEngine& Component::GetPhysicsEngine() const
    {
        return Engine::Get().GetPhysicsEngine();
    }

    Renderer& Component::GetRenderer() const
    {
        return Engine::Get().GetRenderer();
    }

    InputManager& Component::GetInputManager() const
    {
        return Engine::Get().GetInputManager();
    }

    SceneManager& Component::GetSceneManager() const
    {
        return Engine::Get().GetSceneManager();
    }

    WindowManager& Component::GetWindowManager() const
    {
        return  Engine::Get().GetWindowManager();
    }
#pragma endregion
}
