#pragma once

#include "Core.hpp"
#include "ShaderStage.hpp"

#include "vulkan/vulkan.h"

#include <vector>

#include "VK_Device.hpp"

namespace Rigel::Backend::Vulkan
{
    class VK_Device;

    class VK_ShaderModule
    {
    public:
        explicit VK_ShaderModule(const std::vector<char>& sourceBytes);
        ~VK_ShaderModule();

        VK_ShaderModule(const VK_ShaderModule& other) = delete;
        VK_ShaderModule& operator = (const VK_ShaderModule&) = delete;

        NODISCARD inline VkShaderModule Get() const { return m_ShaderModule; }
        NODISCARD VkPipelineShaderStageCreateInfo GetStageInfo(const ShaderStage stage);
    private:
        VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo m_ShaderStageInfo;
    };
}
