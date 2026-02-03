#pragma once

#include "objects.hpp"

#include <glm/glm.hpp>

#include <thread>

class Simulation;
class ParticleWorld;

extern "C" {
    typedef void (*callback_t)(void* data, int count);

    struct Context {
        bool shouldStop{};

        std::mutex mutex;

        std::vector<glm::vec3> positions[2];

        Simulation* simulation{};
        ParticleData particleData;
        SimulationMode simulationMode;

        callback_t callback{};
    };

    void init();

    void setData(void* positions, void* velocities, int count);

    void setResultReadyCallback(callback_t callback);

    void end();
}