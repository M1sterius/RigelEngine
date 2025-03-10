cmake_minimum_required(VERSION 3.28)

project(RigelEngine)

add_library(RigelEngine STATIC
    Source/Engine.cpp
    Source/EngineTools/Time.cpp
    Source/Subsystems/SceneManager.cpp
    Source/Subsystems/Renderer/Renderer.cpp
    Source/Debug/Debug.cpp
    Source/Debug/Logger.cpp
    Source/Utils/Serialization/GLM_Serializer.cpp
    Source/ECS/Scene.cpp
    Source/ECS/GameObject.cpp
    Source/ECS/Component.cpp
    Source/Components/Transform.cpp
    Source/ECS/SceneHandle.cpp
    Source/ECS/GOHandle.cpp
    Source/Subsystems/EventSystem/EventManager.cpp
    Source/ECS/ComponentTypeRegistry.cpp
    Source/Utils/Filesystem/File.cpp
    Source/Subsystems/AssetManager/AssetManager.cpp
    Source/Subsystems/WindowManager.cpp
    Source/Backend/Renderer/Vulkan/VK_Renderer.cpp
    Source/EngineTools/Input.cpp
    Source/Subsystems/InputManager.cpp
    Source/Utils/Filesystem/Directory.cpp
    Source/Debug/Exceptions.cpp
    Source/Backend/Renderer/Vulkan/VK_Instance.cpp
    Source/Backend/Renderer/Vulkan/MakeInfo.cpp
    Source/Backend/Renderer/Vulkan/VK_Device.cpp
    Source/Backend/Renderer/Vulkan/VK_Surface.cpp
    Source/Backend/Renderer/Vulkan/VK_Swapchain.cpp
    Source/Backend/Renderer/Vulkan/VK_Fence.cpp
    Source/Backend/Renderer/Vulkan/VK_Semaphore.cpp
    Source/Backend/Renderer/Vulkan/VK_CmdBuffer.cpp
    Source/Backend/Renderer/Vulkan/VK_MemoryBuffer.cpp
)

set_target_properties(RigelEngine PROPERTIES
    PREFIX ""
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 20
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Engine/${CMAKE_BUILD_TYPE}
)

set(PUBLIC_COMPILE_DEFINITIONS "RIGEL_ENGINE")
set(LINK_LIBRARIES)

if(WIN32)
    list(APPEND LINK_LIBRARIES winmm)
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "RGE_PLATFORM_WINDOWS")
elseif(APPLE)
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "RGE_PLATFORM_MACOS")
elseif(UNIX)
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "RGE_PLATFORM_LINUX")
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
    Include/Utils/PCH/STD_PCH.hpp
    Include/Utils/PCH/RGE_PCH.hpp
)

set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)

set(VULKAN_SDK $ENV{VULKAN_SDK})
set(VULKAN_SDK_PATH "${VULKAN_SDK}")

# Include paths that will only be visible inside the library
target_include_directories(RigelEngine PRIVATE
    Source/Utils/Internal
    Source/Backend
    Source/Backend/Renderer/Vulkan

    ${Vulkan_INCLUDE_DIRS}
    ${VULKAN_SDK_PATH}/Include/vulkan
    ${DEPENDENCIES_DIR}/glfw/include/GLFW
)

# Include paths that should be available outside to the projects linking the engine
target_include_directories(RigelEngine PUBLIC
    ${DEPENDENCIES_DIR}/nlohmann_json
    ${DEPENDENCIES_DIR}/glm

    Include
    Include/Debug
    Include/ECS
    Include/Components
    Include/Utils
    Include/Utils/Filesystem
    Include/Utils/Math
    Include/Subsystems
    Include/Subsystems/AssetManager
    Include/Subsystems/Renderer
    Include/Subsystems/EventSystem
    Include/Utils/HeaderUtils
    Include/Utils/Serialization
    Include/EngineTools
)

# compile glfw library from source
add_subdirectory(${DEPENDENCIES_DIR}/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)

find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(RigelEngine PUBLIC
    ${LINK_LIBRARIES} glfw Vulkan::Vulkan OpenGL::GL
)

## copy .dll file
#set(DLL_DIR ${CMAKE_SOURCE_DIR}/Bin/Engine/${CMAKE_BUILD_TYPE}/RigelEngine.dll)
#set(DLL_COPY_OUT_DIR ${CMAKE_SOURCE_DIR}/Bin/Sandbox/${CMAKE_BUILD_TYPE})
#add_custom_command(TARGET RigelEngine POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    ${DLL_DIR}
#    ${DLL_COPY_OUT_DIR}
#)
