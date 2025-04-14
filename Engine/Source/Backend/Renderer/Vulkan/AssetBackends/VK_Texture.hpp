#pragma once

#include "Core.hpp"
#include "ITextureBackend.hpp"

#include <filesystem>
#include <memory>

namespace Rigel::Backend::Vulkan
{
    class VK_Image;

    class VK_Texture final : public ITextureBackend
    {
    public:
        explicit VK_Texture(std::filesystem::path path);
        ~VK_Texture() override;
    private:
        std::unique_ptr<VK_Image> m_Image;
    };

}
