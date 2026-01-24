#pragma once

#include "objects.hpp"

#include <vector>

class Simulation {
public:
    Simulation() = delete;
    Simulation(std::vector<Particle>& particles, std::vector<Plane>& obstacles);

    void update(float deltaTime);

private:
    std::vector<Plane>* m_obstacles;
    std::vector<Particle>* m_particles;

    void applyForces(Particle& particle, float deltaTime);
    void clampVelocity(Particle& particle);
    void moveParticle(Particle& particle, float deltaTime);
    void resolveCollisions(Particle& particle);
};
