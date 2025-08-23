#include "VK_StagingManager.hpp"
#include "VK_Device.hpp"
#include "VK_MemoryBuffer.hpp"
#include "Subsystems/SubsystemGetters.hpp"
#include "Subsystems/AssetManager/AssetManager.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_StagingManager::VK_StagingManager(VK_Device& device)
        : m_Device(device)
    {
        m_Buffers[std::this_thread::get_id()] = std::make_unique<VK_MemoryBuffer>(m_Device, MB(4), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);

        for (const auto& id : GetAssetManager()->GetLoadingThreadsIDs())
        {
            m_Buffers[id] = std::make_unique<VK_MemoryBuffer>(m_Device, MB(4), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_TO_GPU);
        }
    }

    VK_StagingManager::~VK_StagingManager() = default;

    VK_MemoryBuffer& VK_StagingManager::GetBuffer() const
    {
        const auto thisThreadID = std::this_thread::get_id();
        if (!m_Buffers.contains(thisThreadID))
        {
            Debug::Crash(ErrorCode::VULKAN_UNRECOVERABLE_ERROR,
                "Staging buffer can only be retrieved for one of asset manager's loading threads or the main thread!", __FILE__, __LINE__);
        }

        return *m_Buffers.at(thisThreadID);
    }
}
