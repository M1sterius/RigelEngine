cmake_minimum_required(VERSION 3.29)

project(Sandbox)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Sandbox/${CMAKE_BUILD_TYPE})

add_executable(Sandbox
    Source/main.cpp
    Source/TestComponent.cpp
    Source/TestComponent.hpp
)

set(LINK_LIBRARIES RigelEngine)

# statically link necessary runtime libraries when using gcc
if (MINGW)
    list(APPEND LINK_LIBRARIES -static-libgcc -static-libstdc++ -static)
endif ()

target_include_directories(Sandbox PRIVATE ${CMAKE_SOURCE_DIR}/Engine/Include)
target_link_libraries(Sandbox PRIVATE ${LINK_LIBRARIES})
