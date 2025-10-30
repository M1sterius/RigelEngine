cmake_minimum_required(VERSION 3.28)

project(RigelEngine)

set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)
set(ENGINE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Bin/Engine/${CMAKE_BUILD_TYPE})

file(GLOB_RECURSE IMGUI_SOURCES
    "${DEPENDENCIES_DIR}/imgui/*.cpp"
)

add_library(RigelEngine STATIC
    # Dependencies
    ${IMGUI_SOURCES}
    ${DEPENDENCIES_DIR}/stb_image/stb_image.cpp
    ${DEPENDENCIES_DIR}/vma/vma.cpp
    ${DEPENDENCIES_DIR}/tiny_gltf/tiny_gltf.cpp

    # Misc
    Source/Utilities/Math/Math.cpp
    Source/Utilities/Math/Random.cpp
    Source/Engine.cpp
    Source/Debug/Debug.cpp
    Source/Debug/Logger.cpp

    # Ecs
    Source/ECS/Scene.cpp
    Source/ECS/GameObject.cpp
    Source/ECS/Component.cpp

    # Handles
    Source/Handles/SceneHandle.cpp
    Source/Handles/GOHandle.cpp

    # Assets
    Source/Assets/Model.cpp
    Source/Assets/Texture.cpp
    Source/Assets/Material.cpp
    Source/Assets/Shader.cpp

    # Components
    Source/Components/Camera.cpp
    Source/Components/Transform.cpp
    Source/Components/ModelRenderer.cpp

    # Utilities
    Include/Utilities/Reflection/TypeRegistry.hpp
    Source/Utilities/Reflection/TypeUtility.cpp
    Source/Utilities/Threading/ThreadPool.cpp
    Source/Utilities/Filesystem/Directory.cpp
    Source/Utilities/Filesystem/File.cpp
    Source/Utilities/Threading/SleepUtility.cpp
    Source/Utilities/Serialization/Serializer.cpp
    Source/Utilities/Loaders/GLTF_Loader.cpp

    # Subsystems
    Source/Subsystems/Time.cpp
    Source/Subsystems/SceneManager.cpp
    Source/Subsystems/Renderer/Renderer.cpp
    Source/Subsystems/AssetManager/AssetDeleter.cpp
    Source/Subsystems/SubsystemGetters.cpp
    Source/Subsystems/AssetManager/AssetManager.cpp
    Source/Subsystems/WindowManager.cpp
    Source/Subsystems/InputManager/InputManager.cpp
    Source/Subsystems/EventSystem/EventManager.cpp
    Source/Subsystems/PhysicsEngine/PhysicsEngine.cpp

    # Vulkan
    Source/Backend/Renderer/Vulkan/ImGui/VK_ImGUI_Renderer.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_ShaderModule.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Image.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_GraphicsPipeline.cpp
    Source/Backend/Renderer/Vulkan/VK_BindlessManager.cpp
    Source/Backend/Renderer/Vulkan/AssetBackends/VK_Texture.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_DescriptorSet.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_DescriptorPool.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Instance.cpp
    Source/Backend/Renderer/Vulkan/Helpers/MakeInfo.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Device.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Surface.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Swapchain.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Fence.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Semaphore.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_CmdBuffer.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_MemoryBuffer.cpp
    Source/Backend/Renderer/Vulkan/VK_Renderer.cpp
    Source/Backend/Renderer/Vulkan/VK_MeshPool.cpp
    Source/Backend/Renderer/Vulkan/AssetBackends/VK_Mesh.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_CmdPool.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_VertexBuffer.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_IndexBuffer.cpp
    Source/Backend/Renderer/Vulkan/VK_GBuffer.cpp
    Source/Backend/Renderer/Vulkan/VK_StagingManager.cpp
    Source/Backend/Renderer/Vulkan/RenderPasses/VK_GeometryPass.cpp
    Source/Backend/Renderer/Vulkan/RenderPasses/VK_LightingPass.cpp
    Source/Backend/Renderer/Vulkan/VK_GPUScene.cpp
    Source/Backend/Renderer/Vulkan/RenderPasses/VK_ForwardPass.cpp
)

set_target_properties(RigelEngine PROPERTIES
    PREFIX ""
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 23
    ARCHIVE_OUTPUT_DIRECTORY ${ENGINE_OUTPUT_PATH}
)

set(PUBLIC_COMPILE_DEFINITIONS "RIGEL_ENGINE")
set(LINK_LIBRARIES)

if(WIN32)
    list(APPEND LINK_LIBRARIES winmm)
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "RIGEL_PLATFORM_WINDOWS")
elseif(APPLE)
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "RIGEL_PLATFORM_MACOS")
elseif(UNIX)
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "RIGEL_PLATFORM_LINUX")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "DEBUG_BUILD")
elseif (CMAKE_BUILD_TYPE STREQUAL "Test")
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "TEST_BUILD")
else ()
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "RELEASE_BUILD")
endif ()

target_compile_definitions(RigelEngine PUBLIC ${PUBLIC_COMPILE_DEFINITIONS})
target_compile_definitions(RigelEngine PRIVATE "RIGEL_INTERNAL")

target_precompile_headers(RigelEngine PRIVATE
    Include/Utilities/PCH/STD_PCH.hpp
    Include/Utilities/PCH/RGE_PCH.hpp
)

# Include paths that should be available to projects linking the engine
target_include_directories(RigelEngine PUBLIC
    Include/Vendor # TODO: Add imgui to vendor

    Include
    Include/Debug
    Include/Utilities/Math
)

set(VULKAN_SDK $ENV{VULKAN_SDK})
set(VULKAN_SDK_PATH "${VULKAN_SDK}")

# Include paths that will only be visible inside the engine project
target_include_directories(RigelEngine PRIVATE
    ${DEPENDENCIES_DIR}
    ${VULKAN_SDK_PATH}/Include
    ${DEPENDENCIES_DIR}/glfw/include
    ${DEPENDENCIES_DIR}/assimp/include
    ${DEPENDENCIES_DIR}/imgui
    ${DEPENDENCIES_DIR}/stb_image
    ${DEPENDENCIES_DIR}/tiny_gltf

    Include/Vendor/nlohmann_json # required for tiny_gltf

    Source
)

# compile dependencies from source
add_subdirectory(${DEPENDENCIES_DIR}/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)

find_package(Vulkan REQUIRED)

target_link_libraries(RigelEngine PUBLIC
    ${LINK_LIBRARIES} glfw Vulkan::Vulkan
)

add_custom_target(RunBuildAssets ALL
    COMMAND python ${CMAKE_SOURCE_DIR}/Engine/Assets/BuildAssets.py ${CMAKE_SOURCE_DIR}/Engine/Assets ${CMAKE_BUILD_TYPE} ${ENGINE_OUTPUT_PATH}
    COMMENT "Running build assets python script!"
)

add_dependencies(RigelEngine RunBuildAssets)
