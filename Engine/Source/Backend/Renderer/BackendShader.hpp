#pragma once

#include <filesystem>

namespace rge::backend
{
    class BackendShader
    {
    public:
        BackendShader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath) { }
        virtual ~BackendShader() = default;

        BackendShader(const BackendShader& other) = delete;
        BackendShader& operator = (const BackendShader&) = delete;
    };
}