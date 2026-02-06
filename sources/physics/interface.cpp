#include "interface.hpp"

#include "renderer.hpp"
#include "simulation.hpp"

#include <glad.h>

#include <raylib.h>
#include <rlgl.h>

#include <iostream>

Context context{};

/*
void uploadData();
void downloadData();
void swapBuffers();
*/

/*
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

void swapBuffers() { std::swap(context.positions[0], context.positions[1]); }
*/

void init() {
    InitWindow(300, 200, "window");

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

    context.camera = new Camera3D();
    context.camera->position   = Vector3{0, 10, 10};
    context.camera->target     = Vector3{0, 0, 0};
    context.camera->up         = Vector3{0, 1, 0};
    context.camera->fovy       = 45.f;
    context.camera->projection = CAMERA_PERSPECTIVE;

    RendererContext rendererContext{
        .simulationMode = context.simulationMode,

        .camera = context.camera,

        .planeData = nullptr,
        .particleData = &context.particleData,
    };

    context.renderer = new Renderer(rendererContext);
}

void update(float deltaTime) {
    context.simulation->update(deltaTime);

    //downloadData();
    //swapBuffers();

    //if (context.callback) {
    //    context.callback(context.positions[0].data(), context.particleData.count);
    //}

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
        auto pos = glm::vec3(context.particleData.position[i]);
        posData[i] = pos;
        std::cout << pos.x << ' ' << pos.y << ' ' << pos.z << std::endl;
        std::cout << posData[i].x << ' ' << posData[i].y << ' ' << posData[i].z << std::endl;
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