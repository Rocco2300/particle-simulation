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

static glm::vec3 getRandomDirection() {
    static std::mt19937 gen(std::random_device{}());
    static std::normal_distribution<float> dist(-1.0f, 1.0f);

    return glm::normalize(glm::vec3(dist(gen), dist(gen), dist(gen)));
}

static glm::vec3 getRandomPosition() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<float> distVert(1.0f, 9.0f);
    static std::uniform_real_distribution<float> distHori(-4.0f, 4.0f);

    return glm::normalize(glm::vec3(distHori(gen), distVert(gen), distHori(gen)));
}


static constexpr std::array<Color, 7> colors {RED, BLUE, GREEN, YELLOW, MAGENTA, VIOLET, ORANGE};

static Color getRandomColor() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 6);

    return colors[dist(gen)];
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

    SetTargetFPS(60);

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

    const int ParticleNumber = 100;
    Particle particle;
    std::vector<Particle> particles;
    for (int i = 0; i < ParticleNumber; i++) {
        particle.radius    = 0.25f;
        particle.position  = getRandomPosition();
        particle.velocity  = getRandomDirection() * 10.f;
        particle.color     = getRandomColor();

        particles.push_back(particle);
    }

    Renderer renderer(particles, obstacles);
    Simulation simulation(particles, obstacles);

    bool movingCam{};
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

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

            DrawText("Welcome to the 3rd dimension", 10, 40, 20, DARKGRAY);
            DrawFPS(10, 10);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}