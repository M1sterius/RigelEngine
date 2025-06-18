#pragma once

#include "Core.hpp"

#include "vulkan.h"

#include <filesystem>
#include <memory>

namespace Rigel::Backend::Vulkan
{
    class VK_Image;

    class VK_Texture final
    {
    public:
        explicit VK_Texture(std::filesystem::path path);
        ~VK_Texture();

        NODISCARD VK_Image& GetImage() const { return *m_Image; }
        NODISCARD VkSampler GetSampler() const { return m_Sampler; }
    private:
        std::unique_ptr<VK_Image> m_Image;
        VkSampler m_Sampler = VK_NULL_HANDLE;
        const std::filesystem::path m_Path;
        glm::uvec2 m_Size {};

        uint32_t m_BindlessIndex = -1;
    };

}
