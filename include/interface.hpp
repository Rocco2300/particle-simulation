#pragma once

#include "objects.hpp"
#include "renderer.hpp"

class Simulation;
class ParticleWorld;

extern "C" {
    typedef void (*empty_callback_t)();
    typedef void (*callback_t)(void* data, int count);

    struct Context {
        bool shouldStop{};

        Camera3D* camera{};
        Renderer* renderer{};
        Simulation* simulation{};

        ParticleData particleData;
        SimulationMode simulationMode;
    };

    void init();

    void update(float deltaTime);

    void setData(void* positions, void* velocities, int count);

    void getData(void* positions);

    void printPositions();

    void printVelocities();

    void clean();
}