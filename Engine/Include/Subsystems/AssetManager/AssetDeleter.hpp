#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

namespace Rigel::Backend
{
    class AssetDeleter
    {
    public:
        explicit AssetDeleter(RigelAsset* ptr);
        ~AssetDeleter();
    private:
        RigelAsset* m_Asset;
    };
}