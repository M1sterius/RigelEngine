#include "HandleValidator.hpp"

namespace Rigel::Backend::HandleValidation
{
    void HandleValidator::AddAssetHandle(const uid_t handle)
    {
        m_AssetHandles.emplace(handle);
    }

    void HandleValidator::RemoveAssetHandle(const uid_t handle)
    {
        for (auto it = m_AssetHandles.begin(); it != m_AssetHandles.end(); ++it)
        {
            if (*it == handle)
            {
                m_AssetHandles.erase(it);
                return;
            }
        }
    }

    bool HandleValidator::ValidateAssetHandle(const uid_t handle)
    {
        for (auto it = m_AssetHandles.begin(); it != m_AssetHandles.end(); ++it)
        {
            if (*it == handle)
                return true;
        }

        return false;
    }
}
