#include "VK_ShaderModule.hpp"
#include "../Helpers/VulkanUtility.hpp"

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

    VK_ShaderModule::VK_ShaderModule(const std::vector<byte_t>& spirv)
        : m_ShaderStageInfo()
    {
        auto shaderInfo = MakeInfo<VkShaderModuleCreateInfo>();
        shaderInfo.codeSize = spirv.size();
        shaderInfo.pCode = reinterpret_cast<const uint32_t*>(spirv.data());

        VK_CHECK_RESULT(vkCreateShaderModule(GetDevice().Get(), &shaderInfo, nullptr, &m_ShaderModule), "Failed to create vertex shader module!");

        m_ShaderStageInfo = MakeInfo<VkPipelineShaderStageCreateInfo>();
        m_ShaderStageInfo.module = m_ShaderModule;
        m_ShaderStageInfo.pName = "main";
    }

    VK_ShaderModule::~VK_ShaderModule()
    {
        vkDestroyShaderModule(GetDevice().Get(), m_ShaderModule, nullptr);
    }

    void VK_ShaderModule::SetStage(const ShaderStage stage)
    {
        m_ShaderStageInfo.stage = DeduceShaderStageFlag(stage);
    }
}
