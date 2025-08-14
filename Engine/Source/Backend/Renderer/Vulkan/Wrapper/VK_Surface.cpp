#include "VK_Surface.hpp"
#include "VulkanUtility.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/WindowManager/WindowManager.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Rigel::Backend::Vulkan
{

    VK_Surface::VK_Surface(VkInstance instance)
        : m_Instance(instance)
    {
        Debug::Trace("Creating window surface.");

        const auto glfwWindow = GetWindowManager()->GetGLFWWindowPtr();

        VK_CHECK_RESULT(glfwCreateWindowSurface(instance, glfwWindow, nullptr, &m_Surface), "Failed to create window surface!");
    }

    VK_Surface::~VK_Surface()
    {
        Debug::Trace("Destroying window surface.");

        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    }
}