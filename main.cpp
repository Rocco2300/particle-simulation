#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <raylib.h>
#include <raymath.h>

#include <iostream>
#include <vector>

Mesh mesh;
Model model;
Texture texture;

Vector3 toVector3(glm::vec3 value) { return {value.x, value.y, value.z}; }

struct Plane {
    glm::vec2 size;
    glm::vec3 position;
    glm::vec3 normal;
};

struct Particle {
    float radius;
    glm::vec3 position;
};

void drawParticle(const Particle& particle) {
    DrawSphere(toVector3(particle.position), particle.radius, BLUE);
}

void drawParticles(const std::vector<Particle*>& particles) {
    for (const auto& particle: particles) {
        drawParticle(*particle);
    }
}

glm::mat4 getPlaneTransform(const Plane& plane) {
    auto translate = glm::translate({1}, plane.position);
    auto rotation  = glm::mat4_cast(glm::rotation({0, 1, 0}, plane.normal));
    auto scale     = glm::scale({1}, glm::vec3{plane.size.x, 1, plane.size.y});

    return translate * rotation * scale;
}

Matrix getMatrix(const glm::mat4& m) {
    Matrix r;

    r.m0 = m[0][0];
    r.m1 = m[0][1];
    r.m2 = m[0][2];
    r.m3 = m[0][3];

    r.m4 = m[1][0];
    r.m5 = m[1][1];
    r.m6 = m[1][2];
    r.m7 = m[1][3];

    r.m8  = m[2][0];
    r.m9  = m[2][1];
    r.m10 = m[2][2];
    r.m11 = m[2][3];

    r.m12 = m[3][0];
    r.m13 = m[3][1];
    r.m14 = m[3][2];
    r.m15 = m[3][3];

    return r;
}

void drawPlane(const Plane& plane) {
    auto transform  = getPlaneTransform(plane);
    model.transform = getMatrix(transform);

    DrawModel(model, {0, 0, 0}, 1, WHITE);
}

void updatePosition(Particle& particle, float dt) { particle.position += glm::vec3(0, -1, 0) * dt; }

void updateParticles(std::vector<Particle*>& particles, float dt) {
    for (auto& particle: particles) {
        updatePosition(*particle, dt);
    }
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

    Particle p1{.5f, {0, 3, 0}};
    Particle p2{.5f, {5, 3, 0}};
    Particle p3{.5f, {-5, 3, 0}};

    std::vector<Particle*> particles;
    particles.push_back(&p1);
    particles.push_back(&p2);
    particles.push_back(&p3);

    mesh  = GenMeshPlane(1, 1, 1, 1);
    model = LoadModelFromMesh(mesh);

    auto image = GenImageColor(1, 1, RED);
    texture    = LoadTextureFromImage(image);

    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Plane plane{{3, 3}, {3, 0, 0}, {1, 0, 0}};

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        updateParticles(particles, dt);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
            {
                drawParticles(particles);
                drawPlane(plane);
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