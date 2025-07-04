#include "VK_Surface.hpp"
#include "VulkanUtility.hpp"
#include "WindowManager.hpp"

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

namespace Rigel::Backend::Vulkan
{

    VK_Surface::VK_Surface(VkInstance instance)
        : m_Instance(instance)
    {
        Debug::Trace("Creating window surface.");

        const auto glfwWindow = Engine::Get().GetWindowManager().GetGLFWWindowPtr();

        VK_CHECK_RESULT(glfwCreateWindowSurface(instance, glfwWindow, nullptr, &m_Surface), "Failed to create window surface!");
    }

    VK_Surface::~VK_Surface()
    {
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

        Debug::Trace("Window surface destroyed.");
    }
}