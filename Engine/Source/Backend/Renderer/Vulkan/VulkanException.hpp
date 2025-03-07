#pragma once

#include "Core.hpp"
#include "Debug.hpp"
#include "Exceptions.hpp"

namespace rge::backend
{
    /**
     * A little helper function that converts VkResult enumerated code into its string representation
     */
    NODISCARD inline static const char* VkResultToString(const VkResult result)
    {
        static const std::unordered_map<VkResult, const char*> resultStrings = {
            {VK_SUCCESS,                                            "VK_SUCCESS"},
            {VK_NOT_READY,                                          "VK_NOT_READY"},
            {VK_TIMEOUT,                                            "VK_TIMEOUT"},
            {VK_EVENT_SET,                                          "VK_EVENT_SET"},
            {VK_EVENT_RESET,                                        "VK_EVENT_RESET"},
            {VK_INCOMPLETE,                                         "VK_INCOMPLETE"},
            {VK_ERROR_OUT_OF_HOST_MEMORY,                           "VK_ERROR_OUT_OF_HOST_MEMORY"},
            {VK_ERROR_OUT_OF_DEVICE_MEMORY,                         "VK_ERROR_OUT_OF_DEVICE_MEMORY"},
            {VK_ERROR_INITIALIZATION_FAILED,                        "VK_ERROR_INITIALIZATION_FAILED"},
            {VK_ERROR_DEVICE_LOST,                                  "VK_ERROR_DEVICE_LOST"},
            {VK_ERROR_MEMORY_MAP_FAILED,                            "VK_ERROR_MEMORY_MAP_FAILED"},
            {VK_ERROR_LAYER_NOT_PRESENT,                            "VK_ERROR_LAYER_NOT_PRESENT"},
            {VK_ERROR_EXTENSION_NOT_PRESENT,                        "VK_ERROR_EXTENSION_NOT_PRESENT"},
            {VK_ERROR_FEATURE_NOT_PRESENT,                          "VK_ERROR_FEATURE_NOT_PRESENT"},
            {VK_ERROR_INCOMPATIBLE_DRIVER,                          "VK_ERROR_INCOMPATIBLE_DRIVER"},
            {VK_ERROR_TOO_MANY_OBJECTS,                             "VK_ERROR_TOO_MANY_OBJECTS"},
            {VK_ERROR_FORMAT_NOT_SUPPORTED,                         "VK_ERROR_FORMAT_NOT_SUPPORTED"},
            {VK_ERROR_FRAGMENTED_POOL,                              "VK_ERROR_FRAGMENTED_POOL"},
            {VK_ERROR_UNKNOWN,                                      "VK_ERROR_UNKNOWN"},
            {VK_ERROR_OUT_OF_POOL_MEMORY,                           "VK_ERROR_OUT_OF_POOL_MEMORY"},
            {VK_ERROR_INVALID_EXTERNAL_HANDLE,                      "VK_ERROR_INVALID_EXTERNAL_HANDLE"},
            {VK_ERROR_FRAGMENTATION,                                "VK_ERROR_FRAGMENTATION"},
            {VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS,               "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"},
            {VK_PIPELINE_COMPILE_REQUIRED,                          "VK_PIPELINE_COMPILE_REQUIRED"},
            {VK_ERROR_SURFACE_LOST_KHR,                             "VK_ERROR_SURFACE_LOST_KHR"},
            {VK_ERROR_NATIVE_WINDOW_IN_USE_KHR,                     "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"},
            {VK_SUBOPTIMAL_KHR,                                     "VK_SUBOPTIMAL_KHR"},
            {VK_ERROR_OUT_OF_DATE_KHR,                              "VK_ERROR_OUT_OF_DATE_KHR"},
            {VK_ERROR_INCOMPATIBLE_DISPLAY_KHR,                     "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"},
            {VK_ERROR_VALIDATION_FAILED_EXT,                        "VK_ERROR_VALIDATION_FAILED_EXT"},
            {VK_ERROR_INVALID_SHADER_NV,                            "VK_ERROR_INVALID_SHADER_NV"},
            {VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR,                "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR"},
            {VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR,       "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR"},
            {VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR,    "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR"},
            {VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR,       "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR"},
            {VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR,        "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR"},
            {VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR,          "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR"},
            {VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT, "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"},
            {VK_ERROR_NOT_PERMITTED_KHR,                            "VK_ERROR_NOT_PERMITTED_KHR"},
            {VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT,          "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"},
            {VK_THREAD_IDLE_KHR,                                    "VK_THREAD_IDLE_KHR"},
            {VK_THREAD_DONE_KHR,                                    "VK_THREAD_DONE_KHR"},
            {VK_OPERATION_DEFERRED_KHR,                             "VK_OPERATION_DEFERRED_KHR"},
            {VK_OPERATION_NOT_DEFERRED_KHR,                         "VK_OPERATION_NOT_DEFERRED_KHR"},
            {VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR,             "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR"},
            {VK_ERROR_COMPRESSION_EXHAUSTED_EXT,                    "VK_ERROR_COMPRESSION_EXHAUSTED_EXT"},
            {VK_INCOMPATIBLE_SHADER_BINARY_EXT,                     "VK_INCOMPATIBLE_SHADER_BINARY_EXT"},
            {VK_PIPELINE_BINARY_MISSING_KHR,                        "VK_PIPELINE_BINARY_MISSING_KHR"},
            {VK_ERROR_NOT_ENOUGH_SPACE_KHR,                         "VK_ERROR_NOT_ENOUGH_SPACE_KHR"},
            {VK_ERROR_OUT_OF_POOL_MEMORY_KHR,                       "VK_ERROR_OUT_OF_POOL_MEMORY_KHR"},
            {VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR,                  "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR"},
            {VK_ERROR_FRAGMENTATION_EXT,                            "VK_ERROR_FRAGMENTATION_EXT"},
            {VK_ERROR_NOT_PERMITTED_EXT,                            "VK_ERROR_NOT_PERMITTED_EXT"},
            {VK_ERROR_INVALID_DEVICE_ADDRESS_EXT,                   "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT"},
            {VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR,           "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR"},
            {VK_PIPELINE_COMPILE_REQUIRED_EXT,                      "VK_PIPELINE_COMPILE_REQUIRED_EXT"},
            {VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT,                "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT"}
        };

        const auto it = resultStrings.find(result);
        return (it != resultStrings.end()) ? it->second : "UNKNOWN_VK_RESULT";
    }

    /**
     * A custom class for vulkan specific exceptions
     */
    class VulkanException : public RigelException
    {
    public:
        VulkanException(const std::string& what, const VkResult result)
                : RigelException(std::format("VulkanError: {}. Result: {}", what, VkResultToString(result)))
        {

        }
    };
}