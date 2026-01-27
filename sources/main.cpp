#include "objects.hpp"
#include "renderer.hpp"
#include "simulation.hpp"
#include "particle_manager.hpp"

#include <raylib.h>
#include <rlgl.h>

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

    PlaneData planeData;
    planeData.count   = 6;
    planeData.size[0] = {10, 10};
    planeData.size[1] = {10, 10};
    planeData.size[2] = {10, 10};
    planeData.size[3] = {10, 10};
    planeData.size[4] = {10, 10};
    planeData.size[5] = {10, 10};

    planeData.normal[0] = {0, 1, 0, 0};
    planeData.normal[1] = {0, -1, 0, 0};
    planeData.normal[2] = {1, 0, 0, 0};
    planeData.normal[3] = {-1, 0, 0, 0};
    planeData.normal[4] = {0, 0, 1, 0};
    planeData.normal[5] = {0, 0, -1, 0};

    planeData.position[0] = {0, 0, 0, 0};
    planeData.position[1] = {0, 10, 0, 0};
    planeData.position[2] = {-5, 5, 0, 0};
    planeData.position[3] = {5, 5, 0, 0};
    planeData.position[4] = {0, 5, -5, 0};
    planeData.position[5] = {0, 5, 5, 0};

    // clang-format off
    global.planes.sizeSSBO = rlLoadShaderBuffer(sizeof(PlaneData::size), nullptr, RL_DYNAMIC_COPY);
    global.planes.normalSSBO = rlLoadShaderBuffer(sizeof(PlaneData::normal), nullptr, RL_DYNAMIC_COPY);
    global.planes.positionSSBO = rlLoadShaderBuffer(sizeof(PlaneData::position), nullptr, RL_DYNAMIC_COPY);

    global.particles.radiusSSBO = rlLoadShaderBuffer(sizeof(ParticleData::radius), nullptr, RL_DYNAMIC_COPY);
    global.particles.positionSSBO = rlLoadShaderBuffer(sizeof(ParticleData::position), nullptr, RL_DYNAMIC_COPY);
    global.particles.velocitySSBO = rlLoadShaderBuffer(sizeof(ParticleData::velocity), nullptr, RL_DYNAMIC_COPY);
    global.particles.accelerationSSBO = rlLoadShaderBuffer(sizeof(ParticleData::acceleration), nullptr, RL_DYNAMIC_COPY);
    global.particles.colorSSBO = rlLoadShaderBuffer(sizeof(ParticleData::color), nullptr, RL_DYNAMIC_COPY);
    // clang-format on

    rlUpdateShaderBuffer(
            global.planes.sizeSSBO,
            &planeData.size,
            sizeof(planeData.size),
            0
    );

    rlUpdateShaderBuffer(
            global.planes.normalSSBO,
            &planeData.normal,
            sizeof(planeData.normal),
            0
    );

    rlUpdateShaderBuffer(
            global.planes.positionSSBO,
            &planeData.position,
            sizeof(planeData.position),
            0
    );

    ParticleManager particleManager;
    auto& particleData = particleManager.particleData();

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
                particleManager.spawn();
                time = SpawnTime;
            }
        }

        if (IsKeyPressed(KEY_C)) {
            particleManager.impulse();
        }

        simulation.update(dt);
        renderer.draw();
    }

    CloseWindow();

    return 0;
}