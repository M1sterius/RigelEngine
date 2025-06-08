#pragma once

#include "Core.hpp"

namespace Rigel::Backend::Vulkan
{
    class VK_Renderer;
    class VK_DescriptorPool;
    class VK_CmdBuffer;
    class VK_Image;

    class VK_ImGUI_Renderer final
    {
    public:
        explicit VK_ImGUI_Renderer(VK_Renderer& renderer);
        ~VK_ImGUI_Renderer();

        ErrorCode Startup();
        ErrorCode Shutdown();

        void BeginNewFrame();
        void RenderFrame(void* optData);
    private:
        VK_Renderer& m_Renderer;

        std::unique_ptr<VK_DescriptorPool> m_DescriptorPool;
    };
}
