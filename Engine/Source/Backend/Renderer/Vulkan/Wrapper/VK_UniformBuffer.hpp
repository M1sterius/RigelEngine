#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include "vulkan.h"

#include <memory>

namespace Rigel::Backend::Vulkan
{
    class VK_Device;
    class VK_MemoryBuffer;

    struct DefaultUBO
    {
        glm::mat4 MVP;
    };

    class VK_UniformBuffer
    {
    public:
        VK_UniformBuffer(VK_Device& device, const size_t size);
        ~VK_UniformBuffer();

        NODISCARD VK_MemoryBuffer& GetMemoryBuffer() const { return *m_MemoryBuffer; }

        void UploadData(const size_t offset, const size_t size, const void* data) const;

        // Assumes that the size of the data occupies the whole buffer
        void UploadData(const void* data) const;
    private:
        VK_Device& m_Device;

        std::unique_ptr<VK_MemoryBuffer> m_MemoryBuffer;
        const size_t m_Size;
    };
}
