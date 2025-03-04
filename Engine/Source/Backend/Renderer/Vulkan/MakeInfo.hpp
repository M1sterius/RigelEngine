#pragma once

#include "Core.hpp"

namespace rge::backend
{
    /// @brief A small helper function that return vulkan create infos with sType already set
    /// @code{.cpp}
    /// auto render_pass_ci = make_info<VkRenderPassCreateInfo>();
    /// @endcode
    /// @note Also zeros the returned struct
    template <typename T>
    NODISCARD T MakeInfo(T = {});

} // namespace inexor::vulkan_renderer::wrapper
