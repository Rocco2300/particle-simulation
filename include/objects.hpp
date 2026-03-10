#pragma once

#include <glm/glm.hpp>

#define BOX_TYPE 1
#define SPHERE_TYPE 0

// structs just used to set ssbo sizes
struct Cell {
    int size{};
    int list[100]{};
};

constexpr int Size = 12;
constexpr int GridNo = Size * Size * Size;

struct Grid {
    Cell cells[GridNo]{};
};

enum class SimulationMode {
    CPU,
    GPU
};

struct PlanesGlobalData {
    uint32_t sizeSSBO;
    uint32_t normalSSBO;
    uint32_t positionSSBO;
};

struct ParticlesGlobalData {
    uint32_t typeSSBO;
    uint32_t radiusSSBO;
    uint32_t positionSSBO;
    uint32_t velocitySSBO;
    uint32_t accelerationSSBO;
    uint32_t colorSSBO;

    uint32_t gridSSBO;
};

struct Global {
    int temp;
    PlanesGlobalData planes;
    ParticlesGlobalData particles;
};

extern Global global;

using Plane = int;
using Particle = int;

constexpr int MaxPlanes = 6;
constexpr int MaxParticles = 5000;

struct PlaneData {
    int count{};
    glm::vec2 size[MaxPlanes];
    glm::vec4 position[MaxPlanes];
    glm::vec4 normal[MaxPlanes];
};

struct ParticleData {
    int count{};
    int type[MaxParticles]{}; // 0 is sphere, 1 is box
    float radius[MaxParticles]{};
    glm::vec4 position[MaxParticles]{};
    glm::vec4 velocity[MaxParticles]{};
    glm::vec4 acceleration[MaxParticles]{};
    glm::vec4 color[MaxParticles]{};
};
