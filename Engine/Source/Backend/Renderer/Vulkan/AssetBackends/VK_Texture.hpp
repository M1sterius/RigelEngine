#pragma once

#include "Core.hpp"
#include "Texture.hpp"

#include "vulkan.h"

#include <filesystem>
#include <memory>

namespace Rigel
{
    class Texture;
}

namespace Rigel::Backend::Vulkan
{
    class VK_Image;

    class VK_Texture final
    {
    public:
        explicit VK_Texture(const void* pixelData, const glm::uvec2 size);
        ~VK_Texture();

        NODISCARD VK_Image& GetImage() const { return *m_Image; }
        NODISCARD glm::uvec2 GetSize() const;
    private:
        std::unique_ptr<VK_Image> m_Image;

        uint32_t m_BindlessIndex = -1;
    };

}
