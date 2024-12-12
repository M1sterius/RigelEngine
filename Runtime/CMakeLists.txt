cmake_minimum_required(VERSION 3.29)

project(RigelEngine)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Runtime/${CMAKE_BUILD_TYPE})

add_library(RigelEngine SHARED
    Source/RigelEngine.cpp
    Source/Core/Core.cpp
    Source/Core/ECS/GameObject.cpp
    Source/Core/Subsystems/Filesystem/File.cpp
    Source/Core/ECS/Component.cpp
    Source/Core/ECS/Scene.cpp
    Source/Core/Subsystems/Filesystem/Filesystem.cpp
    Source/Core/Time.cpp
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

target_include_directories(RigelEngine PUBLIC
    Include
    Include/Core
    Include/Core/ECS
    Include/Core/Subsystems
    Include/Core/Subsystems/Filesystem
    Include/Utils
    Source/Utils
)

# copy .dll file
set(DLL_DIR ${CMAKE_SOURCE_DIR}/Bin/Runtime/${CMAKE_BUILD_TYPE}/RigelEngine.dll)
set(DLL_COPY_OUT_DIR ${CMAKE_SOURCE_DIR}/Bin/Sandbox/${CMAKE_BUILD_TYPE})
add_custom_command(TARGET RigelEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${DLL_DIR}
    ${DLL_COPY_OUT_DIR}
)
