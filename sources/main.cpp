#include "cpu_simulation.hpp"
#include "gpu_simulation.hpp"
#include "objects.hpp"
#include "particle_world.hpp"
#include "renderer.hpp"
#include "simulation.hpp"

#include <raylib.h>

#include <filesystem>
#include <iostream>

int main(int argc, char* argv[]) {
    auto simulationMode = SimulationMode::CPU;
    if (argc >= 2) {
        std::string mode = argv[1];
        if (mode == "--gpu") {
            simulationMode = SimulationMode::GPU;
        }
    }

    bool partition{};
    int particleNo = MaxParticles;
    if (argc >= 3) {
        std::string gridString = argv[2];
        if (gridString == "--partition") {
            partition = true;
        } else {
            particleNo = std::min(std::atoi(argv[2]), MaxParticles);
        }
    }

    if (argc >= 4) {
        particleNo = std::min(std::atoi(argv[3]), MaxParticles);
    }

    const int ScreenWidth  = 800;
    const int ScreenHeight = 600;

    InitWindow(ScreenWidth, ScreenHeight, "window");

    Camera3D camera{};
    camera.position   = Vector3{0, 10, 10};
    camera.target     = Vector3{0, 0, 0};
    camera.up         = Vector3{0, 1, 0};
    camera.fovy       = 45.f;
    camera.projection = CAMERA_PERSPECTIVE;

    ParticleWorld particleWorld;
    particleWorld.buildBoxWorld(particleNo);

    auto projectPath = std::filesystem::current_path().parent_path();
    RendererContext rendererContext{
            .simulationMode = simulationMode,
            .projectPath    = projectPath,

            .camera       = &camera,
            .planeData    = &particleWorld.planeData(),
            .particleData = &particleWorld.particleData()
    };

    SimulationContext simulationContext{
            .simulationMode = simulationMode,
            .projectPath    = projectPath,

            .steps = 3,

            .gravity          = true,
            .planeCollisions  = true,
            .spatialPartition = partition,

            .planeData    = &particleWorld.planeData(),
            .particleData = &particleWorld.particleData()
    };

    Simulation* simulation{};
    if (simulationMode == SimulationMode::GPU) {
        simulation = new GPUSimulation(simulationContext);
    } else {
        simulation = new CPUSimulation(simulationContext);
    }

    Renderer renderer(rendererContext);

    bool movingCam{};
    const float SpawnTime = 0.050f;
    float time            = SpawnTime;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        time -= dt;

        if (IsKeyPressed(KEY_Z) && !movingCam) {
            movingCam = true;
            DisableCursor();
        } else if (IsKeyPressed(KEY_Z) && movingCam) {
            movingCam = false;
            EnableCursor();
        }

        if (movingCam) {
            UpdateCamera(&camera, CAMERA_FREE);
        }

        if (IsKeyPressedRepeat(KEY_X)) {
            if (time <= 0.f) {
                particleWorld.spawn({4, 8, -4}, true);
                time = SpawnTime;
            }
        }

        if (IsKeyPressed(KEY_C)) {
            particleWorld.impulse();
        }

        simulation->update(dt);
        renderer.draw();
    }

    delete simulation;
    CloseWindow();

    return 0;
}