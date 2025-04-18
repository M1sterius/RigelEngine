#pragma once

#include "Core.hpp"
#include "plf_colony.h"

namespace Rigel::Backend::HandleValidation
{
    class HandleValidator 
    {
    public:
        static void AddAssetHandle(const uid_t handle);
        static void RemoveAssetHandle(const uid_t handle);

        NODISCARD static bool ValidateAssetHandle(const uid_t handle);
    private:
        inline static plf::colony<uid_t> m_AssetHandles;
    };
}
