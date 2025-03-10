#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"

namespace rge
{
    class Asset : public RigelObject
    {
    public:
    INTERNAL:
        ~Asset() override = default;
    private:
        explicit Asset(const uid_t id) : RigelObject(id) { }

        friend class AssetManager;
    };
}
