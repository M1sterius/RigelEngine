#pragma once

#include "GOHandle.hpp"
#include "Core.hpp"
#include "RigelObject.hpp"
#include "GameObject.hpp"
#include "ISerializable.hpp"

#include <memory>
#include <string>
#include <vector>

namespace rge
{
    class GOHandle;
    class GameObject;

    class Scene final : public RigelObject, public ISerializable
    {
    public:
        NODISCARD nlohmann::json Serialize() const override;
        bool Deserialize(const nlohmann::json& json) override;

        NODISCARD inline std::string GetName() const { return m_Name; }
        inline void SetName(std::string name) { m_Name = std::move(name); }

        NODISCARD inline size_t GetSize() const { return m_GameObjects.size(); }

        NODISCARD inline bool IsLoaded() const { return m_IsLoaded; }
        NODISCARD bool ValidateGOHandle(const GOHandle& handle) const;

        GOHandle InstantiateGO(std::string name = "GameObject");
        void DestroyGO(const GOHandle& handle);

        /**
         * Searches objects on the scene by given conditional function
         * @param condition Custom function that determines what objects are selected
         * @param countLimit Maximum number of objects that will be returned
         * @return A vector of handles to selected objects
         */
        std::vector<GOHandle> Search(const std::function<bool(const GOHandle&)>& condition, const size_t countLimit = 512) const;
    INTERNAL:
        ~Scene() override = default;

        NODISCARD uid_t GetNextObjectID() { return m_NextObjectID++; }
    private:
        explicit Scene(const uid_t id, std::string name = "New scene");

        void OnLoad(); // Used for initialization logic.
        void OnUnload(); // Used for cleanup logic.

        void OnEndOfFrame();

        std::string m_Name;
        bool m_IsLoaded = false;
        uid_t m_NextObjectID = 1;
        uid_t m_EndOfFrameCallbackID = NULL_ID;

        // TODO: Change to a more optimal data structure (e.g std::hive or plf::colony)
        std::vector<std::unique_ptr<GameObject>> m_GameObjects;

        friend class SceneManager;
    };
}
