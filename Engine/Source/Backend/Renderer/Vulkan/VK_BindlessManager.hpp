#pragma once

#include "Core.hpp"
#include "Texture.hpp"
#include "AssetHandle.hpp"

#include "vulkan.h"

#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Renderer;
    class VK_Device;
    class VK_Texture;
    class VK_DescriptorPool;
    class VK_Image;

    class VK_BindlessManager 
    {
    public:
        static constexpr uint32_t MAX_TEXTURES = 4096;

        VK_BindlessManager(VK_Renderer& renderer, VK_Device& device);
        ~VK_BindlessManager();

        NODISCARD uint32_t AddTexture(const Ref<VK_Texture> texture);
        void RemoveTexture();
    private:
        VK_Renderer& m_Renderer;
        VK_Device& m_Device;

        std::vector<Ref<VK_Texture>> m_Textures;
        std::mutex m_TextureMutex;
    };
}
