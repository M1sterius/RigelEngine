#pragma once

#include <filesystem>
#include <utility>

namespace Rigel::Backend
{
    class IShaderBackend
    {
    public:
        IShaderBackend(std::filesystem::path vertPath, std::filesystem::path fragPath)
            : m_VertPath(std::move(vertPath)), m_FragPath(std::move(fragPath)) { }
        virtual ~IShaderBackend() = default;

        IShaderBackend(const IShaderBackend& other) = delete;
        IShaderBackend& operator = (const IShaderBackend&) = delete;
    protected:
        const std::filesystem::path m_VertPath;
        const std::filesystem::path m_FragPath;
    };
}
