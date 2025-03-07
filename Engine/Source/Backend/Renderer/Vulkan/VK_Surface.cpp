#include "VK_Surface.hpp"
#include "Engine.hpp"
#include "Debug.hpp"
#include "VulkanException.hpp"
#include "WindowManager.hpp"

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

namespace rge::backend
{

    VK_Surface::VK_Surface(VkInstance instance)
        : m_Instance(instance)
    {
        RGE_TRACE("Creating window surface.");

        const auto glfwWindow = Engine::Get().GetWindowManager().GetGLFWWindowPtr();

        if (const auto result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &m_Surface); result != VK_SUCCESS)
            throw VulkanException("Failed to create Vulkan window surface!", result);
    }

    VK_Surface::~VK_Surface()
    {
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    }
}