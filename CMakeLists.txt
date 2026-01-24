cmake_minimum_required(VERSION 3.31)
project(particle_simulation)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

add_executable(
        particle_simulation
        main.cpp
        sources/renderer.cpp
        sources/simulation.cpp
)

set_property(TARGET particle_simulation PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_include_directories(particle_simulation PRIVATE include)

target_link_libraries(particle_simulation PRIVATE raylib glm)