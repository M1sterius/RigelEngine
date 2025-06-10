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
    private:
        std::unique_ptr<VK_Image> m_Image;

        const std::filesystem::path m_Path;
        glm::uvec2 m_Size {};
    };

}
