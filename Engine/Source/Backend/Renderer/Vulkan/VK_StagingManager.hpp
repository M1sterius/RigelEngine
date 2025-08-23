#pragma once

#include "Core.hpp"

#include <memory>
#include <thread>
#include <unordered_map>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_MemoryBuffer;

    class VK_StagingManager 
    {
    public:
        explicit VK_StagingManager(VK_Device& device);
        ~VK_StagingManager();

        VK_StagingManager(const VK_StagingManager&) = delete;
        VK_StagingManager operator = (const VK_StagingManager&) = delete;

        NODISCARD VK_MemoryBuffer& GetBuffer() const;
    private:
        VK_Device& m_Device;

        std::unordered_map<std::thread::id, std::unique_ptr<VK_MemoryBuffer>> m_Buffers;
    };
}
