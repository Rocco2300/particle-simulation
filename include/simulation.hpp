#pragma once

#include "objects.hpp"

struct SimulationContext {
    bool gravity;
    bool planeCollisions;
    SimulationMode simulationMode;

    PlaneData* planeData;
    ParticleData* particleData;
};

class Simulation {
public:
    Simulation() = delete;
    Simulation(SimulationContext& simulationContext);

    void update(float deltaTime);

private:
    uint32_t m_applyProgram;
    uint32_t m_moveProgram;
    uint32_t m_planeCollisionsProgram;
    uint32_t m_particleCollisionsProgram;

    using Cell = std::vector<int>;// list of id's that are contained in cell
    std::vector<Cell> m_grid;

    bool m_gravity;
    bool m_planeCollisions;

    PlaneData* m_planeData;
    ParticleData* m_particleData;
    SimulationMode m_simulationMode;

    void clearGrid();
    void populateGrid();

    int getIndex(glm::ivec3 triIndex);
    int isInBounds(glm::ivec3 triIndex);
    glm::ivec3 getParticleTriIndex(Particle particle);
    int getParticleIndex(Particle particle);

    void applyForces(Particle particle, float deltaTime);
    void clampVelocity(Particle particle);
    void moveParticle(Particle particle, float deltaTime);

    float getSignedDistance(Particle particle, Plane plane);
    bool isCollidingPlane(Particle particle, Plane plane);
    void solvePlanePenetration(Particle particle, Plane plane);

    bool isCollidingParticle(Particle p1, Particle p2);
    void solveParticlePenetration(Particle p1, Particle p2);

    void resolvePlaneCollisions(Particle particle);
    void resolveParticleCollisions(Particle particle);

    int getInvocationCount() const;

    void gpuApplyForces(float deltaTime);
    void gpuMoveParticles(float deltaTime);
    void gpuResolvePlaneCollisions();
    void gpuResolveParticleCollisions();
};
