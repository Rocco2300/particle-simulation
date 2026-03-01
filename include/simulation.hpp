#pragma once

#include "objects.hpp"

#include <filesystem>

struct SimulationContext {
    SimulationMode simulationMode;
    std::filesystem::path projectPath;

    int steps;

    bool gravity;
    bool planeCollisions;
    bool spatialPartition;

    PlaneData* planeData;
    ParticleData* particleData;
};

class Simulation {
public:
    Simulation() = delete;
    Simulation(SimulationContext& simulationContext)
        : m_projectPath{simulationContext.projectPath}
        , m_simulationMode{simulationContext.simulationMode}
        , m_steps{simulationContext.steps}
        , m_gravity{simulationContext.gravity}
        , m_planeCollisions{simulationContext.planeCollisions}
        , m_spatialPartition{simulationContext.spatialPartition}
        , m_planeData{simulationContext.planeData}
        , m_particleData{simulationContext.particleData} {}

    virtual ~Simulation() = default;

    virtual void update(float deltaTime) = 0;

protected:
    SimulationMode m_simulationMode;
    std::filesystem::path m_projectPath;

    int m_steps;

    bool m_gravity;
    bool m_planeCollisions;
    bool m_spatialPartition;

    PlaneData* m_planeData;
    ParticleData* m_particleData;
};
