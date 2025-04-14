#pragma once

namespace Rigel::Backend
{
    class ITextureBackend
    {
    public:
        explicit ITextureBackend(std::filesystem::path path) : m_Path(std::move(path)) { }
        virtual ~ITextureBackend() = default;

        ITextureBackend(const ITextureBackend& other) = delete;
        ITextureBackend& operator = (const ITextureBackend&) = delete;
    protected:
        const std::filesystem::path m_Path;
        glm::uvec2 m_Size {};
    };
}