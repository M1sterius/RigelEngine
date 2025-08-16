#pragma once

#include "Core.hpp"
#include "../../../../../Include/Assets/Texture2D.hpp"

#include "vulkan/vulkan.h"

#include <filesystem>
#include <memory>

namespace Rigel
{
    class Texture2D;
}

namespace Rigel::Backend::Vulkan
{
    class VK_Image;

    class VK_Texture final
    {
    public:
        VK_Texture(const void* pixelData, const glm::uvec2 size, const uint32_t mipLevelCount);
        ~VK_Texture();

        NODISCARD glm::uvec2 GetSize() const;
        NODISCARD inline VK_Image& GetImage() const { return *m_Image; }
        NODISCARD inline uint32_t GetBindlessIndex() const { return m_BindlessIndex; }
        NODISCARD inline const Texture2D::SamplerProperties& GetSamplerProperties() const { return m_SamplerProperties; }
    private:
        void GenerateMipMaps() const;

        std::unique_ptr<VK_Image> m_Image;
        Texture2D::SamplerProperties m_SamplerProperties{};

        uint32_t m_BindlessIndex = UINT32_MAX;
    };

}
