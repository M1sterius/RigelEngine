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
        explicit VK_Texture(const std::filesystem::path& path);
        ~VK_Texture();

        NODISCARD VK_Image& GetImage() const { return *m_Image; }
        NODISCARD glm::uvec2 GetSize() const;
    private:
        std::filesystem::path m_Path;
        std::unique_ptr<VK_Image> m_Image;

        uint32_t m_BindlessIndex = -1;
    };

}
