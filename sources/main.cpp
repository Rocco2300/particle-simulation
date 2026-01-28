#include "objects.hpp"
#include "particle_world.hpp"
#include "renderer.hpp"
#include "simulation.hpp"

#include <raylib.h>

int main(int argc, char* argv[]) {
    auto simulationMode = SimulationMode::CPU;
    if (argc >= 2) {
        std::string mode = argv[1];
        if (mode == "--gpu") {
            simulationMode = SimulationMode::GPU;
        }
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

    //SetTargetFPS(60);

    ParticleWorld particleWorld;

    particleWorld.addPlane({10, 10}, {0, 0, 0}, {0, 1, 0});
    particleWorld.addPlane({10, 10}, {0, 10, 0}, {0, -1, 0});
    particleWorld.addPlane({10, 10}, {-5, 5, 0}, {1, 0, 0});
    particleWorld.addPlane({10, 10}, {5, 5, 0}, {-1, 0, 0});
    particleWorld.addPlane({10, 10}, {0, 5, -5}, {0, 0, 1});
    particleWorld.addPlane({10, 10}, {0, 5, 5}, {0, 0, -1});

    auto& planeData = particleWorld.planeData();
    auto& particleData = particleWorld.particleData();
    Renderer renderer(simulationMode, camera, particleData, planeData);
    Simulation simulation(simulationMode, particleData, planeData);

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
                particleWorld.spawn();
                time = SpawnTime;
            }
        }

        if (IsKeyPressed(KEY_C)) {
            particleWorld.impulse();
        }

        simulation.update(dt);
        renderer.draw();
    }

    CloseWindow();

    return 0;
}