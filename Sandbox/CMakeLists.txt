cmake_minimum_required(VERSION 3.28)

project(Sandbox)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Sandbox/${CMAKE_BUILD_TYPE})

add_executable(Sandbox
    Source/main.cpp
    Source/TestComponent.cpp
    Source/FreeCamController.cpp
)

set(LINK_LIBRARIES RigelEngine)

# statically link necessary runtime libraries when using gcc
if (MINGW)
    list(APPEND LINK_LIBRARIES -static-libgcc -static-libstdc++ -static)
endif ()

target_include_directories(Sandbox PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Include)
target_link_libraries(Sandbox PRIVATE ${LINK_LIBRARIES})

# Copies all test assets from Sandbox project into the output assets folder
set(COPY_OUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets)
#add_custom_command(TARGET Sandbox POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    ${CMAKE_SOURCE_DIR}/Sandbox/Assets
#    ${COPY_OUT_DIR}
#)

# Copies assets included with the Rigel engine into the output assets folder
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Bin/Engine/${CMAKE_BUILD_TYPE}/Assets
    ${COPY_OUT_DIR}/Engine
)
