#pragma once

#include "simulation.hpp"

class CPUSimulation : public Simulation {
public:
    CPUSimulation(SimulationContext& simulationContext);

    void update(float deltaTime) override;

private:
    using Cell = std::vector<int>;// list of id's that are contained in cell
    std::vector<Cell> m_grid;

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
};