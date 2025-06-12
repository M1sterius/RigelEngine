#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"

#include <filesystem>
#include <utility>

namespace Rigel
{
    /**
     * Base class for all assets managed by Rigel engine
     */
    class RigelAsset : public RigelObject
    {
    public:
        ~RigelAsset() override = default;

        NODISCARD inline std::filesystem::path GetPath() const { return m_Path; }

        NODISCARD inline bool IsReady() const { return m_LoadFinished; }
        NODISCARD inline bool IsInitialized() const { return m_Initialized; }
    protected:
        // The object is always initialized with NULL ID because the asset manager always overrides it
        explicit RigelAsset(std::filesystem::path path) noexcept
            : RigelObject(NULL_ID), m_Path(std::move(path)) { }

        virtual ErrorCode Init() = 0;

        bool m_Initialized = false; // true if Init() was successful
        bool m_LoadFinished = false; // true when Load finishes

        const std::filesystem::path m_Path;
    private:
        friend class AssetManager;
    };
}
