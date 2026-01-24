#include "renderer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <raylib.h>
#include <rlgl.h>

Renderer::Renderer() {
    generatePlaneMesh();
    generateSphereMesh();
}

Renderer::Renderer(std::vector<Particle>& particles, std::vector<Plane>& planes)
    : m_planes{&planes}, m_particles{&particles} {
    generatePlaneMesh();
    generateSphereMesh();
}

void Renderer::draw() {
    if (!m_particles) {
        return;
    }

    for (const auto& plane: *m_planes) {
        drawPlane(plane);
    }

    for (const auto& particle: *m_particles) {
        drawParticle(particle);
    }
}

void Renderer::drawParticle(const Particle& particle) {
    auto scale     = glm::scale({1}, glm::vec3(particle.radius));
    auto translate = glm::translate({1}, particle.position);

    auto transform = translate * scale;
    rlPushMatrix();
    {
        rlMultMatrixf(glm::value_ptr(transform));
        DrawModel(m_sphereModel, {0, 0, 0}, 1, WHITE);
    }
    rlPopMatrix();
}

void Renderer::drawPlane(const Plane& plane) {
    auto translate = glm::translate({1}, plane.position);
    auto rotation  = glm::mat4_cast(glm::rotation({0, 1, 0}, plane.normal));
    auto scale     = glm::scale({1}, glm::vec3{plane.size.x, 1, plane.size.y});
    auto transform = translate * rotation * scale;

    rlPushMatrix();
    {
        rlMultMatrixf(glm::value_ptr(transform));
        DrawModel(m_planeModel, {0, 0, 0}, 1, WHITE);
        DrawModelWires(m_planeModel, {0, 0, 0}, 1, DARKGRAY);
    }
    rlPopMatrix();
}

void Renderer::generatePlaneMesh() {
    Mesh mesh    = GenMeshPlane(1, 1, 1, 1);
    m_planeModel = LoadModelFromMesh(mesh);

    auto image   = GenImageColor(1, 1, LIGHTGRAY);
    auto texture = LoadTextureFromImage(image);

    m_planeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
}

void Renderer::generateSphereMesh() {
    Mesh mesh     = GenMeshSphere(1, 16, 16);
    m_sphereModel = LoadModelFromMesh(mesh);

    auto image   = GenImageColor(1, 1, BLUE);
    auto texture = LoadTextureFromImage(image);

    m_sphereModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
}
