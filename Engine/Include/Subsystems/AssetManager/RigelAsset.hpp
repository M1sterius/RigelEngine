#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"

#include <filesystem>

namespace rge
{
    class RigelAsset : public RigelObject
    {
    public:
        ~RigelAsset() override = default;
    protected:
        explicit RigelAsset(const uid_t id) : RigelObject(id) { }

        std::filesystem::path m_Path;
    private:
        friend class AssetManager;
    };
}
