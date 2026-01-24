#include "simulation.hpp"

#include <array>
#include <exception>
#include <iostream>
#include <utility>

Simulation::Simulation(std::vector<Particle>& particles, std::vector<Plane>& obstacles)
    : m_obstacles{&obstacles}
    , m_particles{&particles} {}

void Simulation::update(float deltaTime) {
    const int steps   = 1;
    auto subDeltaTime = deltaTime / steps;
    for (int i = 1; i <= steps; i++) {
        for (auto& particle: *m_particles) {
            applyForces(particle, deltaTime);
            clampVelocity(particle);
            moveParticle(particle, subDeltaTime);
            resolveCollisions(particle);
        }
    }
}

void Simulation::applyForces(Particle& particle, float deltaTime) {
    const auto gravity = glm::vec3(0, -1, 0) * 9.81f;
    particle.acceleration += gravity * deltaTime;
    particle.velocity += particle.acceleration * deltaTime;
}

void Simulation::clampVelocity(Particle& particle) {
    if (particle.velocity.length() >= 16.f) {
        particle.velocity = glm::normalize(particle.velocity) * 16.f;
    }
}

void Simulation::moveParticle(Particle& particle, float deltaTime) {
    particle.position += particle.velocity * deltaTime;
}

static float getSignedDistance(Particle& particle, Plane& plane) {
    return glm::dot(plane.normal, particle.position - plane.position);
}

static bool isColliding(Particle& particle, Plane& plane) {
    return getSignedDistance(particle, plane) <= particle.radius;
}

static void solvePenetration(Particle& particle, Plane& plane) {
    auto distance   = getSignedDistance(particle, plane);
    auto moveAmount = particle.radius - distance;
    particle.position += plane.normal * moveAmount;
}

static bool isColliding(Particle& p1, Particle& p2) {
    auto distance = glm::distance(p1.position, p2.position);

    if (distance <= p1.radius + p2.radius) {
        return true;
    }

    return false;
}

static void solvePenetration(Particle& p1, Particle& p2) {
    auto normal = glm::normalize(p2.position - p1.position);
    auto distance = glm::distance(p1.position, p2.position);
    auto moveAmount = p1.radius + p2.radius - distance;

    p1.position += normal * -(moveAmount / 2);
    p2.position += normal * (moveAmount / 2);
}

void Simulation::resolveCollisions(Particle& particle) {
    for (auto& plane: *m_obstacles) {
        if (isColliding(particle, plane)) {
            solvePenetration(particle, plane);
            particle.velocity = glm::reflect(particle.velocity, plane.normal) * 0.96f;
        }
    }

    for (auto& secondParticle: *m_particles) {
        if (particle.position != secondParticle.position && isColliding(particle, secondParticle)) {
            auto normal = glm::normalize(secondParticle.position - particle.position);

            solvePenetration(particle, secondParticle);

            particle.velocity       = glm::reflect(particle.velocity, normal) * 0.99f;
            secondParticle.velocity = glm::reflect(secondParticle.velocity, normal) * 0.99f;
        }
    }
}
