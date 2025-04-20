#pragma once

#include "Core.hpp"
#include "RigelHandle.hpp"

#include <mutex>
#include <unordered_map>
#include <unordered_set>

namespace Rigel::Backend::HandleValidation
{
    enum class HandleType : uint32_t
    {
        Unknown = 0,
        AssetHandle = 1,
        SceneHandle = 2,
        GOHandle = 3,
        ComponentHandle = 4,
        MaxValue = 5
    };

    template<HandleType hT>
    inline static constexpr bool IS_HANDLE_TYPE_VALID()
    {
        constexpr auto val = static_cast<uint32_t>(hT);
        return val > static_cast<uint32_t>(HandleType::Unknown) &&
               val < static_cast<uint32_t>(HandleType::MaxValue);
    }

    class HandleValidator
    {
    public:
        template<HandleType hT>
        static void AddHandle(const uid_t handle)
        {
            static_assert(IS_HANDLE_TYPE_VALID<hT>());

            std::unique_lock lock(m_Mutex);
            m_ValidHandles[static_cast<uint32_t>(hT)].insert(handle);
        }

        template<HandleType hT>
        static void RemoveHandle(const uid_t handle)
        {
            static_assert(IS_HANDLE_TYPE_VALID<hT>());

            std::unique_lock lock(m_Mutex);
            m_ValidHandles[static_cast<uint32_t>(hT)].erase(handle);
        }

        template<HandleType hT>
        NODISCARD static bool Validate(const uid_t handle)
        {
            static_assert(IS_HANDLE_TYPE_VALID<hT>());

            std::unique_lock lock(m_Mutex);
            return m_ValidHandles[static_cast<uint32_t>(hT)].contains(handle);
        }
    private:
        inline static std::unordered_map<uint32_t, std::unordered_set<uid_t>> m_ValidHandles{};
        inline static std::mutex m_Mutex{};
    };
}
