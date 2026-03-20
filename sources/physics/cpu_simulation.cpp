#include "cpu_simulation.hpp"

#include <random>
#include <iostream>

// TODO: this is a temp fix
static glm::vec3 getRandomDirection() {
    constexpr std::array<glm::vec3, 8> directions{
                {{1, 1, 1},
                 {1, 1, -1},
                 {-1, 1, 1},
                 {-1, 1, -1}}
    };

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution dist(0, 3);

    return glm::normalize(directions[dist(gen)]);
}

CPUSimulation::CPUSimulation(SimulationContext& simulationContext)
    : Simulation(simulationContext) {
    m_grid.resize(12 * 12 * 12);
    for (int i = 0; i < 12 * 12 * 12; i++) {
        m_grid[i].reserve(100);
    }
}

void CPUSimulation::update(float deltaTime) {
    auto subDeltaTime = deltaTime / m_steps;
    for (int i = 1; i <= m_steps; i++) {
        for (int particle = 0; particle < m_particleData->count; particle++) {
            if (m_gravity) {
                applyForces(particle, subDeltaTime);
            }
        }

        if (m_spatialPartition) {
            clearGrid();
        }

        for (int particle = 0; particle < m_particleData->count; particle++) {
            clampVelocity(particle);

            moveParticle(particle, subDeltaTime);

            if (m_planeCollisions) {
                resolvePlaneCollisions(particle);
            }
        }

        if (m_spatialPartition) {
            populateGrid();
        }

        for (int particle = 0; particle < m_particleData->count; particle++) {
            resolveParticleCollisions(particle);
        }
    }
}

void CPUSimulation::clearGrid() {
    for (int i = 0; i < 12 * 12 * 12; i++) {
        m_grid[i].clear();
    }
}

void CPUSimulation::populateGrid() {
    for (int particle = 0; particle < m_particleData->count; particle++) {
        auto position         = m_particleData->position[particle];
        auto velocity         = m_particleData->velocity[particle];
        auto particleTriIndex = getParticleTriIndex(particle);
        auto index            = getParticleIndex(particle);
        if (index < 0 || index >= 12 * 12 * 12) {
            std::cout << "Fucked up!" << '\n';
            std::cout << position.x << ' ' << position.y << ' ' << position.z << '\n';
            std::cout << velocity.x << ' ' << velocity.y << ' ' << velocity.z << '\n';
            std::cout << particleTriIndex.x << ' ' << particleTriIndex.y << ' '
                      << particleTriIndex.z << '\n';
            continue;
        }

        m_grid[index].push_back(particle);
    }
}

int CPUSimulation::getIndex(glm::ivec3 triIndex) {
    return triIndex.z * (12 * 12) + triIndex.y * 12 + triIndex.x;
}

int CPUSimulation::isInBounds(glm::ivec3 triIndex) {
    auto isInXBounds = triIndex.x >= 0 && triIndex.x < 12;
    auto isInYBounds = triIndex.y >= 0 && triIndex.y < 12;
    auto isInZBounds = triIndex.z >= 0 && triIndex.z < 12;

    return isInXBounds && isInYBounds && isInZBounds;
}

glm::ivec3 CPUSimulation::getParticleTriIndex(Particle particle) {
    auto& position = m_particleData->position[particle];

    auto temp = position;
    temp.x += 6.f;
    temp.y += 1.f;
    temp.z += 6.f;

    glm::ivec3 res{};
    res.x = std::floor(temp.x);
    res.y = std::floor(temp.y);
    res.z = std::floor(temp.z);

    return res;
}

int CPUSimulation::getParticleIndex(Particle particle) {
    auto triIndex = getParticleTriIndex(particle);
    return getIndex(triIndex);
}

void CPUSimulation::applyForces(Particle particle, float deltaTime) {
    const auto gravity = glm::vec4(0, -1, 0, 0) * 9.81f;

    auto& velocity     = m_particleData->velocity[particle];
    auto& acceleration = m_particleData->acceleration[particle];

    acceleration += gravity * deltaTime;
    velocity += acceleration * deltaTime;
}

void CPUSimulation::clampVelocity(Particle particle) {
    auto& velocity = m_particleData->velocity[particle];

    auto length = std::min(glm::length(velocity), 25.f);
    velocity    = glm::normalize(velocity) * length;
}

void CPUSimulation::moveParticle(Particle particle, float deltaTime) {
    auto& position = m_particleData->position[particle];
    auto& velocity = m_particleData->velocity[particle];

    position += velocity * deltaTime;
}

float CPUSimulation::getSignedDistance(Particle particle, Plane plane) {
    // This works for spheres and also for cubes, since we deal with cubes
    // as a positions and "radius" which is half size
    auto& position      = m_particleData->position[particle];
    auto& planeNormal   = m_planeData->normal[plane];
    auto& planePosition = m_planeData->position[plane];

    return glm::dot(planeNormal, position - planePosition);
}

bool CPUSimulation::isCollidingPlane(Particle particle, Plane plane) {
    // This works for spheres and also for cubes, since we deal with cubes
    // as a positions and "radius" which is half size
    auto& radius = m_particleData->radius[particle];

    return getSignedDistance(particle, plane) < radius;
}

void CPUSimulation::solvePlanePenetration(Particle particle, Plane plane) {
    // This works for spheres and also for cubes, since we deal with cubes
    // as a positions and "radius" which is half size and we are axis bound
    auto& radius      = m_particleData->radius[particle];
    auto& position    = m_particleData->position[particle];
    auto& planeNormal = m_planeData->normal[plane];

    auto distance   = getSignedDistance(particle, plane);
    auto moveAmount = radius - distance + 0.0001f;
    position += planeNormal * moveAmount;
}

bool CPUSimulation::isCollidingBoxBox(Particle p1, Particle p2) {
    auto& pos1 = m_particleData->position[p1];
    auto& pos2 = m_particleData->position[p2];
    auto& rad1 = m_particleData->radius[p1];
    auto& rad2 = m_particleData->radius[p2];

    auto xDistance = std::abs(pos1.x - pos2.x);
    auto yDistance = std::abs(pos1.y - pos2.y);
    auto zDistance = std::abs(pos2.z - pos1.z);

    auto xOverlap = xDistance <= rad1 + rad2;
    auto yOverlap = yDistance <= rad1 + rad2;
    auto zOverlap = zDistance <= rad1 + rad2;

    return xOverlap && yOverlap && zOverlap;
}

bool CPUSimulation::isCollidingBoxSphere(Particle p1, Particle p2) {
    auto& boxPos       = m_particleData->position[p1];
    auto& spherePos    = m_particleData->position[p2];
    auto& boxSize      = m_particleData->radius[p1];
    auto& sphereRadius = m_particleData->radius[p2];

    auto minPos     = boxPos - glm::vec4(1.f) * boxSize;
    auto maxPos     = boxPos + glm::vec4(1.f) * boxSize;
    auto clampedPos = glm::clamp(spherePos, minPos, maxPos);

    return glm::distance(spherePos, clampedPos) < sphereRadius;
}

bool CPUSimulation::isCollidingSphereSphere(Particle p1, Particle p2) {
    auto& rad1 = m_particleData->radius[p1];
    auto& rad2 = m_particleData->radius[p2];
    auto& pos1 = m_particleData->position[p1];
    auto& pos2 = m_particleData->position[p2];

    return glm::distance(pos1, pos2) < rad1 + rad2;
}

bool CPUSimulation::isCollidingParticle(Particle p1, Particle p2) {
    auto& type1 = m_particleData->type[p1];
    auto& type2 = m_particleData->type[p2];

    if (type1 == BOX_TYPE && type2 == BOX_TYPE) {
        return isCollidingBoxBox(p1, p2);
    }

    if (type1 == SPHERE_TYPE && type2 == SPHERE_TYPE) {
        return isCollidingSphereSphere(p1, p2);
    }

    if (type1 == BOX_TYPE) {
        return isCollidingBoxSphere(p1, p2);
    }

    return isCollidingBoxSphere(p2, p1);
}

void CPUSimulation::solveBoxBoxPenetration(Particle p1, Particle p2) {
    // TODO: this is kind of iffy, since it doesn't take into account the size of the box
    auto& pos1 = m_particleData->position[p1];
    auto& pos2 = m_particleData->position[p2];
    auto& rad1 = m_particleData->radius[p1];
    auto& rad2 = m_particleData->radius[p2];

    auto xOverlap = pos2.x - pos1.x;
    auto yOverlap = pos2.y - pos1.y;
    auto zOverlap = pos2.z - pos1.z;

    auto overlap  = glm::vec4(xOverlap, yOverlap, zOverlap, 0.0f);
    auto normal   = glm::normalize(overlap);
    auto distance = glm::length(overlap);

    //pos1 += overlap * -(1.0f / 2.0f + 0.0001f);
    pos1 += normal * (-distance / 2.0f + 0.0001f);
}

void CPUSimulation::solveBoxSpherePenetration(Particle p1, Particle p2) {
    // TODO: this is kind of iffy, since it doesn't take into account the size of the sphere
    auto& boxPos       = m_particleData->position[p1];
    auto& spherePos    = m_particleData->position[p2];
    auto& boxSize      = m_particleData->radius[p1];
    auto& sphereRadius = m_particleData->radius[p2];

    auto prevBoxPos    = boxPos;
    auto prevSpherePos = spherePos;

    auto minPos     = boxPos - glm::vec4(1.f) * boxSize;
    auto maxPos     = boxPos + glm::vec4(1.f) * boxSize;
    auto clampedPos = glm::clamp(spherePos, minPos, maxPos);

    auto distance = glm::distance(spherePos, clampedPos);
    if (distance == 0.0f) {
        auto dir = getRandomDirection();
        spherePos += glm::vec4(dir * 0.001f, 0.0f);
        distance = glm::distance(spherePos, clampedPos);
    }

    auto normal   = glm::normalize(spherePos - clampedPos);
    //auto overlap  = spherePos - clampedPos;

    //boxPos += overlap * -(1.0f / 2.0f + 0.0001f);
    boxPos += normal * (-distance / 2.0f + 0.0001f);
}

void CPUSimulation::solveSphereSpherePenetration(Particle p1, Particle p2) {
    auto& rad1 = m_particleData->radius[p1];
    auto& rad2 = m_particleData->radius[p2];
    auto& pos1 = m_particleData->position[p1];
    auto& pos2 = m_particleData->position[p2];

    // bug fix, it apparently can happen, somehow...
    if (pos1 == pos2) {
        std::cout << "they were equal, we fixed it\n";
        auto vel = m_particleData->velocity[p1];
        pos1 += vel * 0.001f;
    }

    auto normal     = glm::normalize(pos2 - pos1);
    auto distance   = glm::distance(pos1, pos2);
    auto moveAmount = rad1 + rad2 - distance;

    pos1 += normal * -(moveAmount / 2.0f + 0.0001f);
}

void CPUSimulation::solveParticlePenetration(Particle p1, Particle p2) {
    auto& type1 = m_particleData->type[p1];
    auto& type2 = m_particleData->type[p2];

    if (type1 == BOX_TYPE && type2 == BOX_TYPE) {
        solveBoxBoxPenetration(p1, p2);
        return;
    }

    if (type1 == SPHERE_TYPE && type2 == SPHERE_TYPE) {
        solveSphereSpherePenetration(p1, p2);
        return;
    }

    if (type1 == BOX_TYPE) {
        solveBoxSpherePenetration(p1, p2);
        return;
    }

    solveBoxSpherePenetration(p2, p1);
}

void CPUSimulation::resolvePlaneCollisions(Particle particle) {
    auto& vel = m_particleData->velocity[particle];

    for (int i = 0; i < m_planeData->count; i++) {
        if (isCollidingPlane(particle, i)) {
            solvePlanePenetration(particle, i);

            auto& normal = m_planeData->normal[i];
            vel          = glm::reflect(vel, normal) * 0.90f;
        }
    }
}

void CPUSimulation::resolveParticleCollisionsSimple(Particle particle) {
    auto& pos1 = m_particleData->position[particle];
    auto& vel1 = m_particleData->velocity[particle];

    for (int i = 0; i < m_particleData->count; i++) {
        auto& pos2 = m_particleData->position[i];
        auto& vel2 = m_particleData->velocity[i];

        if (i != particle && isCollidingParticle(i, particle)) {
            auto normal   = glm::normalize(pos2 - pos1);
            auto distance = glm::distance(pos1, pos2);

            solveParticlePenetration(i, particle);

            vel1 = glm::reflect(vel1, normal) * 0.95f;
        }
    }
}

void CPUSimulation::resolveParticleCollisionsPartitioning(Particle particle) {
    auto& pos1 = m_particleData->position[particle];
    auto& vel1 = m_particleData->velocity[particle];

    auto triIndex = getParticleTriIndex(particle);
    for (int z = -1; z <= 1; z++) {
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                auto offset      = glm::ivec3(x, y, z);
                auto newTriIndex = triIndex + offset;
                if (!isInBounds(newTriIndex)) {
                    continue;
                }

                auto index = getIndex(newTriIndex);
                for (int secondParticle: m_grid[index]) {
                    auto& pos2 = m_particleData->position[secondParticle];

                    if (secondParticle != particle &&
                        isCollidingParticle(secondParticle, particle)) {

                        solveParticlePenetration(secondParticle, particle);

                        auto normal = glm::normalize(pos2 - pos1);
                        vel1        = glm::reflect(vel1, normal) * 0.95f;
                    }
                }
            }
        }
    }
}

void CPUSimulation::resolveParticleCollisions(Particle particle) {

    // TODO: this could be done better
    if (!m_spatialPartition) {
        resolveParticleCollisionsSimple(particle);
    } else {
        resolveParticleCollisionsPartitioning(particle);
    }
}
