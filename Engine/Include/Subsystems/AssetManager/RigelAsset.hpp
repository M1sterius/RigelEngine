#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"
#include "HandleValidator.hpp"

#include <filesystem>
#include <utility>

namespace Rigel
{
    class RigelAsset : public RigelObject
    {
    public:
        ~RigelAsset() override = default;

        NODISCARD inline std::filesystem::path GetPath() const { return m_Path; }
    protected:
        // The object is always initialized with NULL ID because the asset manager always overrides it
        explicit RigelAsset(std::filesystem::path path)
            : RigelObject(NULL_ID), m_Path(std::move(path)) { }

        const std::filesystem::path m_Path;
    private:
        friend class AssetManager;
    };
}
