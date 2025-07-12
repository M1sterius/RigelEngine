#include "VK_BindlessManager.hpp"

namespace Rigel::Backend::Vulkan
{
    VK_BindlessManager::VK_BindlessManager(VK_Renderer& renderer, VK_Device& device)
        : m_Renderer(renderer), m_Device(device)
    {
        Debug::Trace("Creating vulkan bindless resources manager.");
    }

    VK_BindlessManager::~VK_BindlessManager()
    {
        Debug::Trace("Destroying vulkan bindless resources manager.");
    }

    uint32_t VK_BindlessManager::AddTexture(const Ref<VK_Texture> texture)
    {
        uint32_t slotIndex = UINT32_MAX;
        {
            std::unique_lock lock(m_TextureMutex);

            for (uint32_t i = 0; i < m_Textures.size(); ++i)
            {
                if (m_Textures.at(i).IsNull())
                {
                    slotIndex = i;
                    break;
                }
            }
        }

        // haven't found an empty slot
        if (slotIndex == UINT32_MAX)
        {
            slotIndex = m_Textures.size();

            if (slotIndex >= MAX_TEXTURES)
            {
                Debug::Crash(ErrorCode::LIMIT_EXCEEDED, "Exceeded the maximum number of bindless textures!", __FILE__, __LINE__);
            }

            {
                std::unique_lock lock(m_TextureMutex);
                m_Textures.push_back(nullptr);
            }
        }

        {
            std::unique_lock lock (m_TextureMutex);
            m_Textures[slotIndex] = texture;
        }

        // update the descriptor set at slotIndex

        return slotIndex;
    }

    void VK_BindlessManager::RemoveTexture()
    {

    }
}
