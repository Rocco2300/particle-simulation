#include "objects.hpp"
#include "renderer.hpp"
#include "simulation.hpp"

#include <raylib.h>
#include <rlgl.h>

#include <random>
#include <vector>

static constexpr float Offset = 1.0f;
static constexpr std::array<Color, 7> colors{RED, BLUE, GREEN, YELLOW, MAGENTA, VIOLET, ORANGE};

static glm::vec4 getRandomColor() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 6);

    auto rayColor = colors[dist(gen)];
    return {rayColor.r / 255.0f, rayColor.g / 255.0f, rayColor.b / 255.0f, rayColor.a / 255.0f};
}

static glm::vec3 getRandomVectorOffset() {
    static std::mt19937 gen(std::random_device{}());
    static std::normal_distribution<float> dist(-1.f, 1.f);

    return glm::vec3(dist(gen), dist(gen), dist(gen));
}

static glm::vec3 getRandomDirection() {
    constexpr std::array<glm::vec3, 8> directions{
            {{1, 1, 1},
             {1, 1, -1},
             {1, -1, 1},
             {1, -1, -1},
             {-1, 1, 1},
             {-1, 1, -1},
             {-1, -1, 1},
             {-1, -1, -1}}
    };

    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 7);

    auto direction = directions[dist(gen)];
    direction += getRandomVectorOffset();

    return glm::normalize(direction);
}

int main() {
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
    planeData.count = 6;
    planeData.size[0] = {10, 10};
    planeData.size[1] = {10, 10};
    planeData.size[2] = {10, 10};
    planeData.size[3] = {10, 10};
    planeData.size[4] = {10, 10};
    planeData.size[5] = {10, 10};

    planeData.normal[0] = {0, 1, 0};
    planeData.normal[1] = {0, -1, 0};
    planeData.normal[2] = {1, 0, 0};
    planeData.normal[3] = {-1, 0, 0};
    planeData.normal[4] = {0, 0, 1};
    planeData.normal[5] = {0, 0, -1};

    planeData.position[0] = {0, 0, 0};
    planeData.position[1] = {0, 10, 0};
    planeData.position[2] = {-5, 5, 0};
    planeData.position[3] = {5, 5, 0};
    planeData.position[4] = {0, 5, -5};
    planeData.position[5] = {0, 5, 5};

    ParticleData particleData;
    for (float y = 1.0f; y <= 9.0f; y += Offset) {
        for (float x = -4.0f; x <= 4.0f; x += Offset) {
            for (float z = -4.0f; z <= 4.0f; z += Offset) {
                particleData.radius[particleData.count]   = 0.25f;
                particleData.position[particleData.count] = glm::vec3(x, y, z);
                particleData.velocity[particleData.count] = getRandomDirection() * 20.f;
                particleData.color[particleData.count]    = getRandomColor();

                particleData.count++;
            }
        }
    }

    // clang-format off
    global.radiusSSBO = rlLoadShaderBuffer(sizeof(particleData.radius), nullptr, RL_DYNAMIC_COPY);
    global.positionSSBO = rlLoadShaderBuffer(sizeof(particleData.position), nullptr, RL_DYNAMIC_COPY);
    global.velocitySSBO = rlLoadShaderBuffer(sizeof(particleData.velocity), nullptr, RL_DYNAMIC_COPY);
    global.accelerationSSBO = rlLoadShaderBuffer(sizeof(particleData.acceleration), nullptr, RL_DYNAMIC_COPY);
    global.colorSSBO = rlLoadShaderBuffer(sizeof(particleData.color), nullptr, RL_DYNAMIC_COPY);
    // clang-format on

    Renderer renderer(camera, particleData, planeData);
    Simulation simulation(particleData, planeData);

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
                auto position = glm::vec3(4, 8, -4);
                auto velocity =
                        glm::normalize(glm::vec3(-1, -1, 1) + getRandomVectorOffset()) * 20.f;

                particleData.radius[particleData.count]   = 0.25f;
                particleData.position[particleData.count] = position;
                particleData.velocity[particleData.count] = velocity;
                particleData.color[particleData.count]    = getRandomColor();

                time = SpawnTime;
                particleData.count++;
            }
        }

        if (IsKeyPressed(KEY_C)) {
            for (int i = 0; i < particleData.count; i++) {
                particleData.velocity[i] = getRandomDirection() * 100.f;
            }
        }

        simulation.update(dt);
        renderer.draw();
    }

    CloseWindow();

    return 0;
}