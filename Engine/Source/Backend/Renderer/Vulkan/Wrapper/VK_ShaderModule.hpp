#pragma once

#include "Core.hpp"
#include "ShaderModuleType.hpp"

#include "vulkan.h"

#include <vector>

#include "VK_Device.hpp"

namespace Rigel::Backend::Vulkan
{
    class VK_Device;

    class VK_ShaderModule
    {
    public:
        VK_ShaderModule(const std::vector<char>& sourceBytes, const ShaderModuleType shaderType);
        ~VK_ShaderModule();

        VK_ShaderModule(const VK_ShaderModule& other) = delete;
        VK_ShaderModule& operator = (const VK_ShaderModule&) = delete;

        NODISCARD inline VkShaderModule Get() const { return m_ShaderModule; }
        NODISCARD inline VkPipelineShaderStageCreateInfo GetShaderStageInfo() const { return m_ShaderStageInfo; }
    private:
        ShaderModuleType m_Type;
        VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo m_ShaderStageInfo;
    };
}
