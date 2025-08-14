cmake_minimum_required(VERSION 3.28)

project(RigelEngine)

set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)
set(ENGINE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Bin/Engine/${CMAKE_BUILD_TYPE})

file(GLOB_RECURSE IMGUI_SOURCES
    "${DEPENDENCIES_DIR}/imgui/*.h"
    "${DEPENDENCIES_DIR}/imgui/*.cpp"
)

set(VMA_SOURCES "${DEPENDENCIES_DIR}/vma/vma.cpp")

add_library(RigelEngine STATIC
    ${IMGUI_SOURCES}
    ${VMA_SOURCES}

    Source/Engine.cpp
    Source/Subsystems/Time.cpp
    Source/Subsystems/SceneManager.cpp
    Source/Subsystems/Renderer/Renderer.cpp
    Source/Debug/Debug.cpp
    Source/Debug/Logger.cpp
    Source/Utilities/Serialization/Serializer.cpp
    Source/ECS/Scene.cpp
    Source/ECS/GameObject.cpp
    Source/ECS/Component.cpp
    Source/Components/Transform.cpp
    Source/Handles/SceneHandle.cpp
    Source/Utilities/Threading/SleepUtility.cpp
    Source/Handles/GOHandle.cpp
    Source/Subsystems/EventSystem/EventManager.cpp
    Source/Subsystems/PhysicsEngine/PhysicsEngine.cpp
    Source/Utilities/Filesystem/File.cpp
    Source/Subsystems/AssetManager/AssetManager.cpp
    Source/Subsystems/WindowManager.cpp
    Source/Backend/Renderer/Vulkan/VK_Renderer.cpp
    Source/Subsystems/InputManager/InputManager.cpp
    Source/Utilities/Filesystem/Directory.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Instance.cpp
    Source/Backend/Renderer/Vulkan/MakeInfo.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Device.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Surface.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Swapchain.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Fence.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Semaphore.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_CmdBuffer.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_MemoryBuffer.cpp
    Source/Backend/Renderer/Vulkan/AssetBackends/VK_Shader.cpp
    Source/Assets/Shader.cpp
    Source/Utilities/Threading/ThreadPool.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_Image.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_GraphicsPipeline.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_VertexBuffer.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_IndexBuffer.cpp
    Source/Components/Camera.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_DescriptorPool.cpp
    Source/Components/ModelRenderer.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_DescriptorSet.cpp
    Source/Assets/Model.cpp
    Source/Backend/Renderer/Vulkan/ImGui/VK_ImGUI_Renderer.cpp
    Source/Backend/Editor.cpp
    Source/Assets/Texture.cpp
    Source/Backend/Renderer/Vulkan/AssetBackends/VK_Texture.cpp
    Include/Utilities/Reflection/TypeRegistry.hpp
    Source/Utilities/Reflection/TypeUtility.cpp
    Source/Backend/Renderer/Vulkan/VK_BindlessManager.cpp
    Source/Assets/Material.cpp
    Source/Subsystems/AssetManager/AssetDeleter.cpp
    Source/Subsystems/SubsystemGetters.cpp
    Source/Backend/Renderer/Vulkan/Wrapper/VK_ShaderModule.cpp
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

set(VULKAN_SDK $ENV{VULKAN_SDK})
set(VULKAN_SDK_PATH "${VULKAN_SDK}")

# Include paths that will only be visible inside the library
target_include_directories(RigelEngine PRIVATE
    Source/Backend
    Source/Backend/Renderer
    Source/Backend/Renderer/Vulkan
    Source/Backend/Renderer/Vulkan/Wrapper
    Source/Backend/Renderer/Vulkan/AssetBackends

    ${DEPENDENCIES_DIR}
    ${Vulkan_INCLUDE_DIRS}
    ${VULKAN_SDK_PATH}/Include/vulkan
    ${DEPENDENCIES_DIR}/glfw/include/GLFW

    ${DEPENDENCIES_DIR}/imgui
    ${DEPENDENCIES_DIR}/vma
    ${DEPENDENCIES_DIR}/stb_image
    ${DEPENDENCIES_DIR}/assimp/include
)

# Include paths that should be available outside to the projects linking the engine
target_include_directories(RigelEngine PUBLIC
    ${DEPENDENCIES_DIR}/nlohmann_json
    ${DEPENDENCIES_DIR}/glm
    ${DEPENDENCIES_DIR}/plf

    Include
    Include/Debug
    Include/ECS
    Include/Components
    Include/Handles
    Include/Utilities
    Include/Utilities/Filesystem
    Include/Utilities/Math
    Include/Utilities/Threading
    Include/Utilities/Reflection
    Include/Subsystems
    Include/Subsystems/AssetManager
    Include/Subsystems/Renderer
    Include/Subsystems/EventSystem
    Include/Subsystems/PhysicsEngine
    Include/Subsystems/InputManager
    Include/Subsystems/WindowManager
    Include/Utilities/Serialization
    Include/Assets
)

# compile dependencies from source
add_subdirectory(${DEPENDENCIES_DIR}/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
add_subdirectory(${DEPENDENCIES_DIR}/assimp ${CMAKE_CURRENT_BINARY_DIR}/assimp)

find_package(Vulkan REQUIRED)

target_link_libraries(RigelEngine PUBLIC
    ${LINK_LIBRARIES} glfw Vulkan::Vulkan assimp
)

add_custom_target(RunBuildAssets ALL
    COMMAND python ${CMAKE_SOURCE_DIR}/Engine/Assets/BuildAssets.py ${CMAKE_SOURCE_DIR}/Engine/Assets ${CMAKE_BUILD_TYPE} ${ENGINE_OUTPUT_PATH}
    COMMENT "Running build assets python script!"
)

add_dependencies(RigelEngine RunBuildAssets)
