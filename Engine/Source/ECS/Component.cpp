#include "Component.hpp"
#include "Debug.hpp"
#include "json.hpp"

#include "Engine.hpp"
#include "SceneManager.hpp"
#include "EventManager.hpp"
#include "AssetManager.hpp"
#include "Subsystems/Renderer/Renderer.hpp"

namespace Rigel
{
    // The NULL_ID will be overwritten by GameObject::AddComponent method
    Component::Component() : RigelObject(NULL_ID),
    Engine(Engine::Get()), AssetManager(Engine.GetAssetManager()),
    EventManager(Engine.GetEventManager()), SceneManager(Engine.GetSceneManager()),
    WindowManager(Engine.GetWindowManager())
    {

    }

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
}
