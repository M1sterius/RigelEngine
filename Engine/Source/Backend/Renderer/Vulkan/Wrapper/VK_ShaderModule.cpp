#include "VK_ShaderModule.hpp"
#include "VulkanUtility.hpp"

namespace Rigel::Backend::Vulkan
{
    static VkShaderStageFlagBits DeduceShaderStageFlag(const ShaderStage shaderType)
    {
        switch (shaderType)
        {
        case ShaderStage::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        }

        return VkShaderStageFlagBits();
    }

    VK_ShaderModule::VK_ShaderModule(const std::vector<char>& sourceBytes)
        : m_ShaderStageInfo()
    {
        auto shaderInfo = MakeInfo<VkShaderModuleCreateInfo>();
        shaderInfo.codeSize = sourceBytes.size();
        shaderInfo.pCode = reinterpret_cast<const uint32_t*>(sourceBytes.data());

        VK_CHECK_RESULT(vkCreateShaderModule(GetDevice().Get(), &shaderInfo, nullptr, &m_ShaderModule), "Failed to create vertex shader module!");

        m_ShaderStageInfo = MakeInfo<VkPipelineShaderStageCreateInfo>();
        m_ShaderStageInfo.module = m_ShaderModule;
        m_ShaderStageInfo.pName = "main";
    }

    VK_ShaderModule::~VK_ShaderModule()
    {
        vkDestroyShaderModule(GetDevice().Get(), m_ShaderModule, nullptr);
    }

    VkPipelineShaderStageCreateInfo VK_ShaderModule::GetStageInfo(const ShaderStage stage)
    {
        m_ShaderStageInfo.stage = DeduceShaderStageFlag(stage);
        return m_ShaderStageInfo;
    }
}
