#pragma once

#include <glm/glm.hpp>

struct Plane {
    glm::vec2 size;
    glm::vec3 position;
    glm::vec3 normal;
};

struct Particle {
    float radius{};
    glm::vec3 position{};
    glm::vec3 velocity{};
    glm::vec3 acceleration{};
};
