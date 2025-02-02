cmake_minimum_required(VERSION 3.29)

project(RigelEngine)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Engine/${CMAKE_BUILD_TYPE})

add_library(RigelEngine SHARED
    Source/Engine.cpp
    Source/EngineTools/Time.cpp
    Source/Subsystems/SceneManager.cpp
    Include/Subsystems/SceneManager.hpp
    Source/Utils/Internal/UIDGenerator.cpp
    Source/Subsystems/Renderer.cpp
    Source/EngineTools/Debug.cpp
    Include/EngineTools/Debug.hpp
    Source/Backend/Logger/Logger.cpp
    Include/Utils/Serialization/ISerializable.hpp
    Source/Utils/Serialization/GLM_Serializer.cpp
    Include/Utils/Serialization/GLM_Serializer.hpp
    Source/ECS/Scene.cpp
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

set (DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)

# Include paths that will only be included inside the library
target_include_directories(RigelEngine PRIVATE
    Source/Utils/Internal
)

# Libraries that should be available outside to the projects linking Engine
target_include_directories(RigelEngine PUBLIC
    ${DEPENDENCIES_DIR}/nlohmann_json
    ${DEPENDENCIES_DIR}/glm

    Include
    Include/ECS
    Include/Utils
    Include/Subsystems
    Include/Utils/HeaderUtils
    Include/EngineTools
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
