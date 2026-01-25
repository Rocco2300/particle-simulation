#include "simulation.hpp"

Simulation::Simulation(ParticleData& particleData, PlaneData& planeData)
    : m_planeData{&planeData}
    , m_particleData{&particleData} {}

void Simulation::update(float deltaTime) {
    const int steps   = 10;
    auto subDeltaTime = deltaTime / steps;
    for (int i = 1; i <= steps; i++) {
        for (int particle = 0; particle < m_particleData->count; particle++) {
            applyForces(particle, subDeltaTime);
            clampVelocity(particle);
            moveParticle(particle, subDeltaTime);
            resolveCollisions(particle);
        }
    }
}

void Simulation::applyForces(Particle particle, float deltaTime) {
    const auto gravity = glm::vec3(0, -1, 0) * 9.81f;

    auto& velocity     = m_particleData->velocity[particle];
    auto& acceleration = m_particleData->acceleration[particle];

    acceleration += gravity * deltaTime;
    velocity += acceleration * deltaTime;
}

void Simulation::clampVelocity(Particle particle) {
    auto& velocity = m_particleData->velocity[particle];

    if (velocity.length() >= 25.f) {
        velocity = glm::normalize(velocity) * 16.f;
    }
}

void Simulation::moveParticle(Particle particle, float deltaTime) {
    auto& position = m_particleData->position[particle];
    auto& velocity = m_particleData->velocity[particle];

    position += velocity * deltaTime;
}

float Simulation::getSignedDistance(Particle particle, Plane plane) {
    auto& position      = m_particleData->position[particle];
    auto& planeNormal   = m_planeData->normal[plane];
    auto& planePosition = m_planeData->position[plane];

    return glm::dot(planeNormal, position - planePosition);
}

bool Simulation::isColliding(Particle particle, Plane plane) {
    auto& radius = m_particleData->radius[particle];

    return getSignedDistance(particle, plane) < radius;
}

void Simulation::solvePenetration(Particle particle, Plane plane) {
    auto& radius      = m_particleData->radius[particle];
    auto& position    = m_particleData->position[particle];
    auto& planeNormal = m_planeData->normal[plane];

    auto distance   = getSignedDistance(particle, plane);
    auto moveAmount = radius - distance + 0.0001f;
    position += planeNormal * moveAmount;
}

bool Simulation::isColliding(Particle p1, Particle p2) {
    auto& rad1 = m_particleData->radius[p1];
    auto& rad2 = m_particleData->radius[p2];
    auto& pos1 = m_particleData->position[p1];
    auto& pos2 = m_particleData->position[p2];

    auto distance = glm::distance(pos1, pos2);

    if (distance < rad1 + rad2) {
        return true;
    }

    return false;
}

void Simulation::solvePenetration(Particle p1, Particle p2) {
    auto& rad1 = m_particleData->radius[p1];
    auto& rad2 = m_particleData->radius[p2];
    auto& pos1 = m_particleData->position[p1];
    auto& pos2 = m_particleData->position[p2];

    auto normal     = glm::normalize(pos2 - pos1);
    auto distance   = glm::distance(pos1, pos2);
    auto moveAmount = rad1 + rad2 - distance;

    pos1 += normal * -((moveAmount / 2.0f) + 0.0001f);
    pos2 += normal * ((moveAmount / 2.0f) + 0.0001f);
}

void Simulation::resolveCollisions(Particle particle) {
    auto& pos1 = m_particleData->position[particle];
    auto& vel1 = m_particleData->velocity[particle];

    for (int i = 0; i < m_planeData->count; i++) {
        if (isColliding(particle, i)) {
            solvePenetration(particle, i);

            auto& normal = m_planeData->normal[i];
            vel1 = glm::reflect(vel1, normal) * 0.90f;
        }
    }

    for (int i = 0; i < m_particleData->count; i++) {
        auto& pos2 = m_particleData->position[i];
        auto& vel2 = m_particleData->velocity[i];

        if (i != particle && isColliding(i, particle)) {
            auto normal   = glm::normalize(pos2 - pos1);
            auto distance = glm::distance(pos1, pos2);

            solvePenetration(i, particle);

            vel1 = glm::reflect(vel1, normal) * 0.95f;
            vel2 = glm::reflect(vel2, normal) * 0.95f;
        }
    }
}
