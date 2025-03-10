#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"

namespace rge
{
    class RigelAsset : public RigelObject
    {
    protected:
        explicit RigelAsset(const uid_t id) : RigelObject(id) { }
        ~RigelAsset() override = default;

        friend class AssetManager;
    };
}
