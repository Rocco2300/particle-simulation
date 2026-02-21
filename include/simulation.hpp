#pragma once

#include "objects.hpp"

struct SimulationContext {
    int steps;
    bool gravity;
    bool planeCollisions;
    SimulationMode simulationMode;

    PlaneData* planeData;
    ParticleData* particleData;
};

class Simulation {
public:
    Simulation() = delete;
    Simulation(SimulationContext& simulationContext)
        : m_steps{simulationContext.steps}
        , m_gravity{simulationContext.gravity}
        , m_planeCollisions{simulationContext.planeCollisions}
        , m_planeData{simulationContext.planeData}
        , m_particleData{simulationContext.particleData}
        , m_simulationMode{simulationContext.simulationMode} {}

    virtual ~Simulation() = default;

    virtual void update(float deltaTime) = 0;

protected:
    int m_steps;
    bool m_gravity;
    bool m_planeCollisions;

    PlaneData* m_planeData;
    ParticleData* m_particleData;
    SimulationMode m_simulationMode;
};
