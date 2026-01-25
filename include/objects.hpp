#pragma once

#include <raylib.h>
#include <glm/glm.hpp>

struct Global {
    uint32_t radiusSSBO;
    uint32_t positionSSBO;
    uint32_t velocitySSBO;
    uint32_t accelerationSSBO;
    uint32_t colorSSBO;
};

extern Global global;

struct Plane {
    glm::vec2 size;
    glm::vec3 position;
    glm::vec3 normal;
};

using Particle = int;

constexpr int MaxParticles = 4096;

struct ParticleData {
    int count{};
    float radius[MaxParticles]{};
    glm::vec3 position[MaxParticles]{};
    glm::vec3 velocity[MaxParticles]{};
    glm::vec3 acceleration[MaxParticles]{};
    glm::vec4 color[MaxParticles]{};
};
