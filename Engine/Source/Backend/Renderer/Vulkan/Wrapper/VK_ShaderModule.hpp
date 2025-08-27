#pragma once

#include "Core.hpp"
#include "ShaderStage.hpp"

#include "vulkan/vulkan.h"

#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;

    class VK_ShaderModule
    {
    public:
        explicit VK_ShaderModule(const std::vector<byte_t>& spirv);
        ~VK_ShaderModule();

        VK_ShaderModule(const VK_ShaderModule& other) = delete;
        VK_ShaderModule& operator = (const VK_ShaderModule&) = delete;

        void SetStage(const ShaderStage stage);

        NODISCARD VkShaderModule Get() const { return m_ShaderModule; }
        NODISCARD VkPipelineShaderStageCreateInfo GetStageInfo() const { return m_ShaderStageInfo; }
    private:
        VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo m_ShaderStageInfo;
    };
}
