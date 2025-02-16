cmake_minimum_required(VERSION 3.29)

project(Sandbox)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Sandbox/${CMAKE_BUILD_TYPE})

add_executable(Sandbox
    Source/main.cpp
        Source/TestComponent.cpp
        Source/TestComponent.hpp
)

target_include_directories(Sandbox PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Include)
target_link_libraries(Sandbox PRIVATE RigelEngine)
