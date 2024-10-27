#pragma once

namespace rge
{
    class Core
    {
    public:
        Core(const Core& other) = delete;

        static void Startup();
        static void Shutdown();

        inline static Core* Get() { return m_Instance; }
        inline static bool IsInitialized() { return m_Instance != nullptr; }

        void Print() const;
    private:
        Core() = default;
        ~Core() = default;

        static Core* m_Instance;
    };
}