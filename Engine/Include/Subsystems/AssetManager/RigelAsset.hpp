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

        NODISCARD inline bool IsOK() const { return IsLoadFinished() && IsInitialized(); }
        NODISCARD inline bool IsInitialized() const { return m_Initialized; }

        NODISCARD inline bool IsLoadFinished() const
        {
            std::unique_lock lock(m_CvMutex);
            return m_LoadFinished;
        }

        // Blocks the calling thread until the asset has been fully loaded
        inline void WaitReady() const
        {
            std::unique_lock lock(m_CvMutex);
            m_CV.wait(lock, [this]
            {
                return m_LoadFinished;
            });
        }

        NODISCARD inline std::filesystem::path GetPath() const { return m_Path; }
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

        mutable std::condition_variable m_CV;
        mutable std::mutex m_CvMutex;

        const std::filesystem::path m_Path;
    private:
        friend class AssetManager;
    };
}
