cmake_minimum_required(VERSION 3.28)

project(BuildAssets)

set(SHADER_FILES
    Shaders/Test.vert
    Shaders/Test.frag
)

set(COMPILED_SHADERS_OUTPUT_DIR ${ENGINE_OUTPUT_PATH}/Assets/Shaders/SPIR-V)
set(SHADER_SOURCES_OUTPUT_DIR ${ENGINE_OUTPUT_PATH}/Assets/Shaders/ShaderSource)
file(MAKE_DIRECTORY ${COMPILED_SHADERS_OUTPUT_DIR})

