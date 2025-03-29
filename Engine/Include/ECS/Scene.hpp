#pragma once

#include "GOHandle.hpp"
#include "Core.hpp"
#include "RigelObject.hpp"
#include "GameObject.hpp"
#include "ISerializable.hpp"
#include "plf_colony.h"

#include <memory>
#include <string>
#include <vector>
#include <queue>

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

        GOHandle Instantiate(std::string name = "GameObject");
        void Destroy(const GOHandle& handle);

        /**
         * Destroys the game objects immediately upon invocation of this method.
         * Using this method may come with serious performance penalty. DO NOT
         * use it unless absolutely necessary. Always prefer regular 'Destroy'!
         * @param handle The GameObject to be destroyed
         */
        inline void DestroyImmediately(const GOHandle& handle) { DestroyGOImpl(handle); }

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
        void OnEndOfFrame(); // Used to process GO deletion queue

        void DestroyGOImpl(const GOHandle& handle);

        std::string m_Name;
        bool m_IsLoaded = false;
        uid_t m_NextObjectID = 1;
        uid_t m_EndOfFrameCallbackID = NULL_ID;

        plf::colony<std::unique_ptr<GameObject>> m_GameObjects;
        std::queue<GOHandle> m_DestroyQueue;

        friend class SceneManager;
    };
}
