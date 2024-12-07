#pragma once

#include "RGE_PCH.hpp"

namespace rge
{
    /**
     * @brief
     */
    class Core
    {
    public:
        Core(const Core& other) = delete;

        static void Startup();
        static void Shutdown();

        inline static Core* Get() { return m_Instance; }
        inline static bool IsInitialized() { return m_Instance != nullptr; }

        void EngineUpdate();
        NODISCARD bool IsRunning() const { return m_IsRunning; }
    private:
        Core() = default;
        ~Core() = default;

        static Core* m_Instance;

        bool m_IsRunning = false;
    };
}