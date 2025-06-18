#pragma once

#include "Core.hpp"
#include "vulkan.h"

namespace Rigel::Backend::Vulkan
{
    class VK_Renderer;
    class VK_Device;
    class VK_Texture;
    class VK_DescriptorPool;

    class TextureRegistry 
    {
    public:
        static constexpr uint32_t MAX_TEXTURES = 2048;

        explicit TextureRegistry(VK_Renderer& renderer);
        ~TextureRegistry();

        NODISCARD uint32_t AddTexture(VK_Texture* texture);
        void RemoveTexture(uint32_t textureIndex);

        NODISCARD VkDescriptorSet GetDescriptorSet() const;
        NODISCARD VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
    private:
        VK_Renderer& m_Renderer;
        VK_Device& m_Device;

        VkDescriptorSet m_DescriptorSet;
        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;

        std::vector<VK_Texture*> m_Registry;
        mutable std::mutex m_RegistryMutex;

        void CreateDescriptorSetLayout();
        VkDescriptorSet CreateDescriptorSet() const;
    };
}
