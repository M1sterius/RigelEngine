cmake_minimum_required(VERSION 3.28)

project(RigelEditor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/RigelEditor/${CMAKE_BUILD_TYPE})

add_executable(RigelEditor
    Source/main.cpp
)
