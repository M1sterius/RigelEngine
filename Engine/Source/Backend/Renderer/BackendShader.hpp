#pragma once

#include <filesystem>
#include <utility>

namespace rge::backend
{
    class BackendShader
    {
    public:
        BackendShader(std::filesystem::path vertPath, std::filesystem::path fragPath)
            : m_VertPath(std::move(vertPath)), m_FragPath(std::move(fragPath)) { }
        virtual ~BackendShader() = default;

        BackendShader(const BackendShader& other) = delete;
        BackendShader& operator = (const BackendShader&) = delete;
    protected:
        const std::filesystem::path m_VertPath;
        const std::filesystem::path m_FragPath;
    };
}
