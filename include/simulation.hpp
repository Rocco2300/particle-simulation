#pragma once

#include "objects.hpp"

#include <vector>

class Simulation {
public:
    Simulation() = delete;
    Simulation(ParticleData& particleData, std::vector<Plane>& obstacles);

    void update(float deltaTime);

private:
    ParticleData* m_particleData;
    std::vector<Plane>* m_obstacles;

    void applyForces(Particle particle, float deltaTime);
    void clampVelocity(Particle particle);
    void moveParticle(Particle particle, float deltaTime);

    float getSignedDistance(Particle particle, Plane& plane);
    bool isColliding(Particle particle, Plane& plane);
    void solvePenetration(Particle particle, Plane& plane);

    bool isColliding(Particle p1, Particle p2);
    void solvePenetration(Particle p1, Particle p2);

    void resolveCollisions(Particle particle);
};
