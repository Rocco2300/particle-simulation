cmake_minimum_required(VERSION 3.30)
project(particle_simulation)

set(CMAKE_CXX_STANDARD 20)
set(GRAPHICS GRAPHICS_API_OPENGL_43)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -g)
endif()

include(FetchContent)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

set(BUILD_SHARED_LIBS ON)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_BRAN
)
FetchContent_MakeAvailable(raylib)

add_subdirectory(sources/physics)

add_executable(
        particle_simulation
        sources/main.cpp
        sources/renderer.cpp
)

add_executable(own_test sources/own_test.cpp)
add_executable(vector_test sources/vector_test.cpp)
add_executable(baseline sources/baseline.cpp)

set_property(TARGET particle_simulation PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_include_directories(particle_simulation PRIVATE include ${CMAKE_BINARY_DIR}/_deps/raylib-src/src/external)

target_link_libraries(particle_simulation PRIVATE raylib glm physics)