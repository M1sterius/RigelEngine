#include "VK_Shader.hpp"
#include "File.hpp"
#include "MakeInfo.hpp"
#include "VulkanException.hpp"

#include "Engine.hpp"
#include "Renderer.hpp"
#include "VK_Device.hpp"
#include "VK_Renderer.hpp"

namespace rge::backend
{
    VK_Shader::VK_Shader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath)
        : BackendShader(vertPath, fragPath)
    {
        const auto vertBytes = File(vertPath).ReadBinary();
        const auto fragBytes = File(fragPath).ReadBinary();

        auto vertInfo = MakeInfo<VkShaderModuleCreateInfo>();
        vertInfo.codeSize = vertBytes.size();
        vertInfo.pCode = reinterpret_cast<const uint32_t*>(vertBytes.data());

        auto fragInfo = MakeInfo<VkShaderModuleCreateInfo>();
        fragInfo.codeSize = fragBytes.size();
        fragInfo.pCode = reinterpret_cast<const uint32_t*>(fragBytes.data());

        const auto& renderer = Engine::Get().GetRenderer();
        const auto& backend = dynamic_cast<const VK_Renderer&>(renderer.GetBackend());
        const auto& device = backend.GetDevice();

        if (const auto result = vkCreateShaderModule(device.Get(), &vertInfo, nullptr, &m_VertexModule); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan vertex shader module!", result);
        if (const auto result = vkCreateShaderModule(device.Get(), &fragInfo, nullptr, &m_FragmentModule); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan fragment shader module!", result);

        m_ShaderStagesInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStagesInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        m_ShaderStagesInfo[0].module = m_VertexModule;
        m_ShaderStagesInfo[0].pName = "main";

        m_ShaderStagesInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_ShaderStagesInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        m_ShaderStagesInfo[1].module = m_FragmentModule;
        m_ShaderStagesInfo[1].pName = "main";
    }

    VK_Shader::~VK_Shader()
    {
        const auto& renderer = Engine::Get().GetRenderer();
        const auto& backend = dynamic_cast<const VK_Renderer&>(renderer.GetBackend());
        const auto& device = backend.GetDevice();

        vkDestroyShaderModule(device.Get(), m_VertexModule, nullptr);
        vkDestroyShaderModule(device.Get(), m_FragmentModule, nullptr);
    }
}
