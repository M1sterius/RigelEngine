#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"

namespace rge
{
    class RigelAsset : public RigelObject
    {
    public:
        ~RigelAsset() override = default;
    protected:
        explicit RigelAsset(const uid_t id) : RigelObject(id) { }

        friend class AssetManager;
    };
}
