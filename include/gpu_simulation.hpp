#pragma once

#include "simulation.hpp"

class GPUSimulation : public Simulation {
public:
    GPUSimulation(SimulationContext& simulationContext);

    void update(float deltaTime) override;

private:
    uint32_t m_applyProgram;
    uint32_t m_clampProgram;
    uint32_t m_moveProgram;
    uint32_t m_planeCollisionsProgram;
    uint32_t m_particleCollisionsProgram;

    int getInvocationCount() const;

    void gpuApplyForces(float deltaTime);
    void gpuClampVelocities();
    void gpuMoveParticles(float deltaTime);
    void gpuResolvePlaneCollisions();
    void gpuResolveParticleCollisions();
};