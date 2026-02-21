add_library(
        physics
        SHARED
        objects.cpp
        particle_world.cpp
        gpu_simulation.cpp
        cpu_simulation.cpp
        interface.cpp
        ../renderer.cpp
)

target_include_directories(
        physics
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/_deps/raylib-src/src/external
)

target_link_libraries(physics PRIVATE raylib glm)