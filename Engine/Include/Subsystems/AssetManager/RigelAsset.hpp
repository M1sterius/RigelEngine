#pragma once

#include "Core.hpp"
#include "RigelObject.hpp"
#include "SleepUtility.hpp"
#include "HandleValidator.hpp"

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
        ~RigelAsset() noexcept override
        {
            using namespace Backend::HandleValidation;
            HandleValidator::RemoveHandle<HandleType::AssetHandle>(this->GetID());
        }

        NODISCARD inline std::filesystem::path GetPath() const { return m_Path; }

        NODISCARD inline bool IsOK() const { return m_LoadFinished && m_Initialized; }
        NODISCARD inline bool IsReady() const { return m_LoadFinished; }
        NODISCARD inline bool IsInitialized() const { return m_Initialized; }

        // Blocks the calling thread until the asset has been fully loaded
        inline void WaitReady() const { SleepUtility::ConditionalSleep([this]() -> bool { return m_LoadFinished; }); }
    protected:
        explicit RigelAsset(std::filesystem::path path, const uid_t id) noexcept
            : RigelObject(id), m_Path(std::move(path))
        {
            using namespace Backend::HandleValidation;
            HandleValidator::AddHandle<HandleType::AssetHandle>(this->GetID());
        }

        virtual ErrorCode Init() = 0;

        bool m_Initialized = false; // true if Init() was successful
        bool m_LoadFinished = false; // true when Load finishes

        const std::filesystem::path m_Path;
    private:
        friend class AssetManager;
    };
}
