#include "VK_ShaderModule.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    static VkShaderStageFlagBits DeduceShaderStageFlag(const ShaderModuleType shaderType)
    {
        switch (shaderType)
        {
        case ShaderModuleType::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderModuleType::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        }

        return VkShaderStageFlagBits();
    }

    VK_ShaderModule::VK_ShaderModule(const std::vector<char>& sourceBytes, const ShaderModuleType shaderType)
        : m_Type(shaderType)
    {
        auto shaderInfo = MakeInfo<VkShaderModuleCreateInfo>();
        shaderInfo.codeSize = sourceBytes.size();
        shaderInfo.pCode = reinterpret_cast<const uint32_t*>(sourceBytes.data());

        VK_CHECK_RESULT(vkCreateShaderModule(GetDevice().Get(), &shaderInfo, nullptr, &m_ShaderModule), "Failed to create vertex shader module!");

        m_ShaderStageInfo = MakeInfo<VkPipelineShaderStageCreateInfo>();
        m_ShaderStageInfo.stage = DeduceShaderStageFlag(shaderType);
        m_ShaderStageInfo.module = m_ShaderModule;
        m_ShaderStageInfo.pName = "main";
    }

    VK_ShaderModule::~VK_ShaderModule()
    {
        vkDestroyShaderModule(GetDevice().Get(), m_ShaderModule, nullptr);
    }
}