#pragma once

#include "objects.hpp"

#include <vector>

class Simulation {
public:
    Simulation() = delete;
    Simulation(ParticleData& particleData, PlaneData& obstacles);

    void update(float deltaTime);

private:
    uint32_t m_applyProgram;
    uint32_t m_moveProgram;
    uint32_t m_planeCollisionsProgram;
    uint32_t m_particleCollisionsProgram;

    PlaneData* m_planeData;
    ParticleData* m_particleData;

    void applyForces(Particle particle, float deltaTime);
    void clampVelocity(Particle particle);
    void moveParticle(Particle particle, float deltaTime);

    float getSignedDistance(Particle particle, Plane plane);
    bool isCollidingPlane(Particle particle, Plane plane);
    void solvePlanePenetration(Particle particle, Plane plane);

    bool isCollidingParticle(Particle p1, Particle p2);
    void solveParticlePenetration(Particle p1, Particle p2);

    void resolveCollisions(Particle particle);

    void gpuApplyForces(float deltaTime);
    void gpuMoveParticles(float deltaTime);
    void gpuResolvePlaneCollisions();
    void gpuResolveParticleCollisions();
};
