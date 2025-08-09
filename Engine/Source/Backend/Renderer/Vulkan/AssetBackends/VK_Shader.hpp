#pragma once

#include "Core.hpp"

#include "vulkan.h"

#include <array>
#include <vector>

namespace Rigel::Backend::Vulkan
{
    class VK_Shader final
    {
    public:
        VK_Shader(const std::vector<char>& vertBytes, const std::vector<char>& fragBytes);
        ~VK_Shader();

        VK_Shader(const VK_Shader& other) = delete;
        VK_Shader& operator = (const VK_Shader&) = delete;

        NODISCARD inline VkShaderModule GetVertexModule() const { return m_VertexModule; }
        NODISCARD inline VkShaderModule GetFragmentModule() const { return m_FragmentModule; }
        NODISCARD inline std::array<VkPipelineShaderStageCreateInfo, 2> GetShaderStagesInfo() const { return m_ShaderStagesInfo; }
    private:
        VkShaderModule m_VertexModule = VK_NULL_HANDLE;
        VkShaderModule m_FragmentModule = VK_NULL_HANDLE;

        std::array<VkPipelineShaderStageCreateInfo, 2> m_ShaderStagesInfo {};
    };
}
