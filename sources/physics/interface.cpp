#include "interface.hpp"

#include "renderer.hpp"
#include "simulation.hpp"

#include <glad.h>

#include <raylib.h>
#include <rlgl.h>

#include <iostream>

Context context{};

void uploadData();
void downloadData();
void swapBuffers();

void uploadData() {
    rlUpdateShaderBuffer(
            global.particles.positionSSBO,
            context.particleData.position,
            sizeof(context.particleData.position),
            0
    );

    rlUpdateShaderBuffer(
            global.particles.velocitySSBO,
            context.particleData.velocity,
            sizeof(context.particleData.velocity),
            0
    );
}

void downloadData() {
    if (context.simulationMode == SimulationMode::GPU) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, global.particles.positionSSBO);
        glGetBufferSubData(
                GL_SHADER_STORAGE_BUFFER,
                0,
                sizeof(context.particleData.position),
                &context.particleData.position
        );
    }
}

void init() {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(300, 200, "window");

    std::fill_n(context.particleData.radius, MaxParticles, 1);

    global.particles.radiusSSBO =
            rlLoadShaderBuffer(sizeof(ParticleData::radius), nullptr, RL_DYNAMIC_COPY);
    global.particles.positionSSBO =
            rlLoadShaderBuffer(sizeof(ParticleData::position), nullptr, RL_DYNAMIC_COPY);
    global.particles.velocitySSBO =
            rlLoadShaderBuffer(sizeof(ParticleData::velocity), nullptr, RL_DYNAMIC_COPY);
    global.particles.accelerationSSBO =
            rlLoadShaderBuffer(sizeof(ParticleData::acceleration), nullptr, RL_DYNAMIC_COPY);

    rlUpdateShaderBuffer(
            global.particles.velocitySSBO,
            context.particleData.velocity,
            sizeof(context.particleData.velocity),
            0
    );

    rlUpdateShaderBuffer(
            global.particles.positionSSBO,
            context.particleData.position,
            sizeof(context.particleData.position),
            0
    );

    rlUpdateShaderBuffer(
            global.particles.radiusSSBO,
            context.particleData.radius,
            sizeof(context.particleData.radius),
            0
    );

    rlUpdateShaderBuffer(
            global.particles.accelerationSSBO,
            context.particleData.acceleration,
            sizeof(context.particleData.acceleration),
            0
    );

    rlUpdateShaderBuffer(
            global.particles.colorSSBO,
            context.particleData.color,
            sizeof(context.particleData.color),
            0
    );

    context.simulationMode = SimulationMode::GPU;
    SimulationContext simulationContext{
            .gravity         = false,
            .planeCollisions = false,
            .simulationMode  = context.simulationMode,

            .planeData    = nullptr,
            .particleData = &context.particleData,
    };

    context.simulation = new Simulation(simulationContext);

    context.camera             = new Camera3D();
    context.camera->position   = Vector3{0, 10, 10};
    context.camera->target     = Vector3{0, 0, 0};
    context.camera->up         = Vector3{0, 1, 0};
    context.camera->fovy       = 45.f;
    context.camera->projection = CAMERA_PERSPECTIVE;

    RendererContext rendererContext{
            .simulationMode = context.simulationMode,

            .camera = context.camera,

            .planeData    = nullptr,
            .particleData = &context.particleData,
    };

    context.renderer = new Renderer(rendererContext);
}

void update(float deltaTime) {
    uploadData();

    context.simulation->update(deltaTime);

    downloadData();

    context.renderer->draw();
}

void setData(void* positions, void* velocities, int count) {
    context.particleData.count = count;

    auto* posData = static_cast<glm::vec3*>(positions);
    auto* velData = static_cast<glm::vec3*>(velocities);

    for (int i = 0; i < count; i++) {
        context.particleData.position[i] = glm::vec4(posData[i], 0.0f);
        context.particleData.velocity[i] = glm::vec4(velData[i], 0.0f);
    }
}

void getData(void* positions) {
    auto* posData = static_cast<glm::vec3*>(positions);
    for (int i = 0; i < context.particleData.count; i++) {
        auto pos   = glm::vec3(context.particleData.position[i]);
        posData[i] = pos;
    }
}

void clean() {
    CloseWindow();

    delete context.renderer;
    delete context.simulation;
}

void test(void* data, int count) {
    auto* intData = static_cast<int*>(data);

    for (int i = 0; i < count; i++) {
        intData[i] = i;
        std::cout << intData[i] << std::endl;
    }
}

void printPositions() {
    std::cout.flush();
    for (int i = 0; i < context.particleData.count; i++) {
        auto& pos = context.particleData.position[i];
        std::cout << pos.x << ' ' << pos.y << ' ' << pos.z << std::endl;
    }
}

void printVelocities() {
    std::cout.flush();
    for (int i = 0; i < context.particleData.count; i++) {
        auto& vel = context.particleData.velocity[i];
        std::cout << vel.x << ' ' << vel.y << ' ' << vel.z << std::endl;
    }
}