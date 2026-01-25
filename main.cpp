#include "objects.hpp"
#include "renderer.hpp"
#include "simulation.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <raylib.h>
#include <rcamera.h>
#include <rlgl.h>

#include <random>
#include <vector>

static constexpr float Offset = 1.0f;
static constexpr std::array<Color, 7> colors{RED, BLUE, GREEN, YELLOW, MAGENTA, VIOLET, ORANGE};

static Color getRandomColor() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 6);

    return colors[dist(gen)];
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

    Plane bottomPlane{{10, 10}, {0, 0, 0}, {0, 1, 0}};
    Plane topPlane{{10, 10}, {0, 10, 0}, {0, -1, 0}};
    Plane leftPlane({10, 10}, {-5, 5, 0}, {1, 0, 0});
    Plane rightPlane({10, 10}, {5, 5, 0}, {-1, 0, 0});
    Plane backPlane({10, 10}, {0, 5, -5}, {0, 0, 1});
    Plane frontPlane({10, 10}, {0, 5, 5}, {0, 0, -1});

    std::vector<Plane> obstacles;
    obstacles.push_back(bottomPlane);
    obstacles.push_back(topPlane);
    obstacles.push_back(leftPlane);
    obstacles.push_back(rightPlane);
    obstacles.push_back(backPlane);
    obstacles.push_back(frontPlane);

    ParticleData particleData;
    for (float y = 1.0f; y <= 9.0f; y += Offset) {
        for (float x = -4.0f; x <= 4.0f; x += Offset) {
            for (float z = -4.0f; z <= 4.0f; z += Offset) {
                particleData.radius[particleData.count]   = 0.25f;
                particleData.position[particleData.count] = glm::vec3(x, y, z);
                particleData.velocity[particleData.count] = getRandomDirection() * 20.f;
                particleData.color[particleData.count]    = getRandomColor();

                particleData.count ++;
            }
        }
    }

    Renderer renderer(particleData, obstacles);
    Simulation simulation(particleData, obstacles);

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
            }
        }

        if (IsKeyPressed(KEY_C)) {
            for (int i = 0; i < particleData.count; i++) {
                particleData.velocity[i] = getRandomDirection() * 100.f;
            }
        }

        simulation.update(dt);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            {
                renderer.draw();
                DrawGrid(100, 1.f);
            }
            EndMode3D();

            DrawFPS(10, 10);
            std::string particleNo = std::to_string(particleData.count);
            DrawText(particleNo.c_str(), 10, 36, 24, DARKGREEN);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}