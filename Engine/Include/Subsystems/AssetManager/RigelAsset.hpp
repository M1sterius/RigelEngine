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

        NODISCARD inline std::filesystem::path GetPath() const { return m_Path; }
    protected:
        // The object is always initialized with NULL ID because the asset manager always overrides it
        explicit RigelAsset(const std::filesystem::path& path)
        : RigelObject(NULL_ID), m_Path(path) { }

        const std::filesystem::path m_Path;
    private:
        friend class AssetManager;
    };
}
