#pragma once

#include <raylib.h>
#include <glm/glm.hpp>

struct PlanesGlobalData {
    uint32_t sizeSSBO;
    uint32_t normalSSBO;
    uint32_t positionSSBO;
};

struct ParticlesGlobalData {
    uint32_t radiusSSBO;
    uint32_t positionSSBO;
    uint32_t velocitySSBO;
    uint32_t accelerationSSBO;
    uint32_t colorSSBO;
};

struct Global {
    PlanesGlobalData planes;
    ParticlesGlobalData particles;
};

extern Global global;

using Plane = int;
using Particle = unsigned int;

constexpr int MaxPlanes = 6;
constexpr int MaxParticles = 4096;

struct PlaneData {
    int count{};
    glm::vec2 size[MaxPlanes];
    glm::vec3 position[MaxPlanes];
    glm::vec3 normal[MaxPlanes];
};

struct ParticleData {
    int count{};
    float radius[MaxParticles]{};
    glm::vec3 position[MaxParticles]{};
    glm::vec3 velocity[MaxParticles]{};
    glm::vec3 acceleration[MaxParticles]{};
    glm::vec4 color[MaxParticles]{};
};
