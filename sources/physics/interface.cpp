#include "interface.hpp"

#include "simulation.hpp"

#include <glad.h>

#include <raylib.h>
#include <rlgl.h>

Context context{};

void threadInit();
void threadLoop();
void threadCleanup();
void threadFunction();

void uploadData();
void downloadData();
void swapBuffers();

void threadInit() {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(1, 1, "window");

    //context.particleWorld = new ParticleWorld();

    //context.particleWorld->buildSuperflatWorld();

    std::fill_n(context.particleData.radius, MaxParticles, 1);

    context.simulationMode = SimulationMode::CPU;
    SimulationContext simulationContext{
            .gravity         = false,
            .planeCollisions = false,
            .simulationMode  = context.simulationMode,

            .planeData    = nullptr,
            .particleData = &context.particleData,
    };

    context.simulation = new Simulation(simulationContext);
}

void threadLoop() {
    float accumulator    = 0.0f;
    constexpr float Tick = 0.016f;
    while (!WindowShouldClose() || !context.shouldStop) {
        accumulator += GetFrameTime();

        if (accumulator >= Tick) {
            std::lock_guard lock(context.mutex);

            if (context.simulationMode == SimulationMode::GPU) {
                uploadData();
            }

            context.simulation->update(accumulator);

            downloadData();
            swapBuffers();

            if (context.callback) {
                context.callback(context.positions[0].data(), context.particleData.count);
            }

            accumulator = 0.0f;
        }
    }

    threadCleanup();
}

void threadCleanup() { delete context.simulation; }

void threadFunction() {
    threadInit();
    threadLoop();
}

void uploadData() {
    rlUpdateShaderBuffer(
            global.particles.positionSSBO,
            context.particleData.position,
            context.particleData.count * sizeof(glm::vec4),
            0
    );

    rlUpdateShaderBuffer(
            global.particles.velocitySSBO,
            context.particleData.velocity,
            context.particleData.count * sizeof(glm::vec4),
            0
    );
}

void downloadData() {
    if (context.simulationMode == SimulationMode::GPU) {
        glGetBufferSubData(
                GL_SHADER_STORAGE_BUFFER,
                0,
                sizeof(context.particleData.position),
                &context.particleData.position
        );
    }

    context.positions[1].resize(context.particleData.count);
    for (int i = 0; i < context.particleData.count; i++) {
        context.positions[1][i] = context.particleData.position[i];
    }
}

void swapBuffers() {
    std::swap(context.positions[0], context.positions[1]);
}

void init() { std::thread(threadFunction).detach(); }

void setData(void* positions, void* velocities, int count) {
    std::lock_guard lock(context.mutex);

    auto* posData = static_cast<glm::vec3*>(positions);
    auto* velData = static_cast<glm::vec3*>(velocities);

    for (int i = 0; i < count; i++) {
        context.particleData.position[i] = glm::vec4(posData[i], 0.0f);
        context.particleData.velocity[i] = glm::vec4(velData[i], 0.0f);
    }
}

void setResultReadyCallback(callback_t callback) { context.callback = callback; }

void end() { context.shouldStop = true; }