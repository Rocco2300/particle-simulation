#pragma once

#include <raylib.h>
#include <glm/glm.hpp>

struct Plane {
    glm::vec2 size;
    glm::vec3 position;
    glm::vec3 normal;
};

/*
struct Particle {
    float radius{};
    glm::vec3 position{};
    glm::vec3 velocity{};
    glm::vec3 acceleration{};

    Color color;
};
*/

using Particle = int;

constexpr int MaxParticles = 4096;

struct ParticleData {
    int count{};
    float radius[4096]{};
    glm::vec3 position[4096]{};
    glm::vec3 velocity[4096]{};
    glm::vec3 acceleration[4096]{};

    Color color[4096]{};
};
