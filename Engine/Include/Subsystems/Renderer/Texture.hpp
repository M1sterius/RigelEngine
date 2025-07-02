#pragma once

#include "Core.hpp"
#include "RigelAsset.hpp"

#include <filesystem>
#include <memory>

namespace Rigel
{
    namespace Backend::Vulkan
    {
        class VK_Texture;
    }

    class Texture final : public RigelAsset
    {
    public:
        enum class Filter : int32_t
        {
            Nearest = 0,
            Linear = 1,
            Cubic = 1000015000
        };
        enum class AddressMode : int32_t
        {
            Repeat = 0,
            MirroredRepeat = 1,
            ClampToEdge = 2,
            ClampToBorder = 3,
            MirroredClampToEdge = 4
        };
        enum class BorderColor : int32_t
        {
            OpaqueBlack = 3,
            OpaqueWhite = 5
        };
        struct SamplerProperties
        {
            Filter MinFilter = Filter::Linear;
            Filter MagFilter = Filter::Linear;

            AddressMode AddressModeU = AddressMode::Repeat;
            AddressMode AddressModeV = AddressMode::Repeat;

            BorderColor BorderColor = BorderColor::OpaqueBlack;

            NODISCARD inline bool operator == (const SamplerProperties& other) const
            {
                return MinFilter == other.MinFilter &&
                       MagFilter == other.MagFilter &&
                       AddressModeU == other.AddressModeU &&
                       AddressModeV == other.AddressModeV &&
                       BorderColor == other.BorderColor;
            }

            NODISCARD inline bool operator != (const SamplerProperties& other) const
            {
                return !(*this == other);
            }
        };

        ~Texture() override;

        NODISCARD glm::uvec2 GetSize() const;
    INTERNAL:
        NODISCARD inline Ref<Backend::Vulkan::VK_Texture> GetImpl() const { return m_Impl.get(); }
    private:
        explicit Texture(const std::filesystem::path& path, const uid_t id) noexcept;
        ErrorCode Init() override;

        std::unique_ptr<Backend::Vulkan::VK_Texture> m_Impl;

        friend class AssetManager;
    };
}
