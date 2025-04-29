#include "GameObject.hpp"
#include "Debug.hpp"
#include "Scene.hpp"
#include "Transform.hpp"
#include "json.hpp"
#include "ComponentTypeRegistry.hpp"

namespace Rigel
{
    using namespace Backend::HandleValidation;

    GameObject::GameObject(const uid_t id, std::string name)
        : RigelObject(id), m_Name(std::move(name)) { }
    GameObject::~GameObject() = default;

    uid_t GameObject::AssignIDToComponent(Component* ptr)
    {
        const auto id = m_Scene->GetNextObjectID();
        ptr->OverrideID(id);
        return id;
    }

    void GameObject::OnLoad()
    {
        m_Loaded = true;

        for (const auto& component : m_Components | std::views::values)
            component->OnLoad();
    }

    void GameObject::OnStart()
    {
        for (const auto& component : m_Components | std::views::values)
            component->OnStart();
    }

    void GameObject::OnDestroy()
    {
        for (const auto& component : m_Components | std::views::values)
            component->OnDestroy();

        m_Loaded = false;
    }

    nlohmann::json GameObject::Serialize() const
    {
        auto json = nlohmann::json();

        json["ID"] = GetID();
        json["Name"] = GetName();

        for (const auto& component : m_Components | std::views::values)
            json["Components"].push_back(component->Serialize());

        return json;
    }

    bool GameObject::Deserialize(const nlohmann::json& json)
    {
        if (!json.contains("Components") || !json.contains("ID") || !json.contains("Name"))
        {
            Debug::Error("Failed to deserialize Rigel::GameObject! Some of the required data is not present in the json object.");
            return false;
        }

        m_Name = json["Name"].get<std::string>();
        this->OverrideID(json["ID"].get<uid_t>());

        for (const auto& componentJson : json["Components"])
        {
            const auto typeString = componentJson["Type"].get<std::string>();
            const auto cmpPtr = ComponentTypeRegistry::FindType(typeString);

            if (cmpPtr)
            {
                if (!cmpPtr->Deserialize(componentJson))
                {
                    // Debug message?
                    delete cmpPtr;
                    continue;
                }

                cmpPtr->m_Scene = m_Scene;
                cmpPtr->m_GameObject = GOHandle(this, this->GetID());

                // This weird line acquires type_index of derived class type from a base class instance
                const auto derivedTypeIndex = std::type_index(typeid(*cmpPtr));

                m_Components[derivedTypeIndex] = std::unique_ptr<Component>(cmpPtr);
                HandleValidator::AddHandle<HandleType::ComponentHandle>(cmpPtr->GetID());
            }
            else
                Debug::Error("Failed to serialize component of type: {}!", typeString);
        }

        return true;
    }
}
