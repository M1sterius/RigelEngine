cmake_minimum_required(VERSION 3.29)

project(RigelEngine)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Engine/${CMAKE_BUILD_TYPE})

add_library(RigelEngine SHARED
    Source/Engine.cpp
    Source/ECS/GameObject.cpp
    Source/ECS/Component.cpp
    Source/ECS/Scene.cpp
    Source/EngineTools/Time.cpp
    Source/Subsystems/SceneManager.cpp
        Source/Utils/Internal/UIDGenerator.cpp
    Source/Utils/Internal/UIDGenerator.hpp
    Source/ECS/RigelObject.cpp
    Source/Subsystems/Renderer/Renderer.cpp
        Source/EngineTools/Debug.cpp
        Include/EngineTools/Debug.hpp
        Source/Backend/Logger.cpp
)

set_target_properties(RigelEngine PROPERTIES PREFIX "")

set(PUBLIC_COMPILE_DEFINITIONS "RIGEL_ENGINE")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "DEBUG_BUILD")
elseif (CMAKE_BUILD_TYPE STREQUAL "Test")
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "TEST_BUILD")
else ()
    list(APPEND PUBLIC_COMPILE_DEFINITIONS "RELEASE_BUILD")
endif ()

target_compile_definitions(RigelEngine PUBLIC ${PUBLIC_COMPILE_DEFINITIONS})
target_compile_definitions(RigelEngine PRIVATE "RIGEL_INTERNAL")

target_precompile_headers(RigelEngine PUBLIC
    Include/Utils/STD_PCH.hpp
    Include/Utils/RGE_PCH.hpp
)

target_include_directories(RigelEngine PRIVATE
    Include
    Include/ECS
    Include/Core
    Include/Utils
    Include/Subsystems
    Include/Subsystems/Renderer
    Include/EngineTools
    Include/Subsystems/Filesystem
    Include/Subsystems/SceneManager

    Source/Utils/Internal
)

set(LINK_LIBRARIES "")

if(WIN32)
    list(APPEND LINK_LIBRARIES winmm)
endif()

target_link_libraries(RigelEngine PRIVATE
    ${LINK_LIBRARIES}
)

# copy .dll file
set(DLL_DIR ${CMAKE_SOURCE_DIR}/Bin/Engine/${CMAKE_BUILD_TYPE}/RigelEngine.dll)
set(DLL_COPY_OUT_DIR ${CMAKE_SOURCE_DIR}/Bin/Sandbox/${CMAKE_BUILD_TYPE})
add_custom_command(TARGET RigelEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${DLL_DIR}
    ${DLL_COPY_OUT_DIR}
)
