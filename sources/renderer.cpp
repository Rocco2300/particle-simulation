#include "renderer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <raylib.h>
#include <rcamera.h>
#include <rlgl.h>

#include <string>

Renderer::Renderer() {
    generatePlaneMesh();
    generateSphereMesh();
}

Renderer::Renderer(Camera3D& camera, ParticleData& particleData, PlaneData& planeData)
    : m_camera{&camera}
    , m_planeData{&planeData}
    , m_particleData{&particleData} {

    generatePlaneMesh();
    generateSphereMesh();

    m_shader = LoadShader(
            "C:/Users/grigo/repos/particle-simulation/shaders/particle.vert",
            "C:/Users/grigo/repos/particle-simulation/shaders/particle.frag"
    );

    m_viewLoc  = GetShaderLocation(m_shader, "view");
    m_projLoc  = GetShaderLocation(m_shader, "projection");
    m_countLoc = GetShaderLocation(m_shader, "count");
}

void Renderer::draw() {
    if (m_particleData->count == 0) {
        return;
    }

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(*m_camera);
        {
            for (int plane = 0; plane < m_planeData->count; plane++) {
                drawPlane(plane);
            }

            drawParticles();

            DrawGrid(100, 1.f);
        }
        EndMode3D();

        DrawFPS(10, 10);
        std::string particleNo = std::to_string(m_particleData->count);
        DrawText(particleNo.c_str(), 10, 36, 24, DARKGREEN);
    }
    EndDrawing();
}


void Renderer::drawParticle(Particle particle) {
    /*
    auto& color    = m_particleData->color[particle];
    auto& radius   = m_particleData->radius[particle];
    auto& position = m_particleData->position[particle];

    auto scale     = glm::scale({1}, glm::vec3(radius));
    auto translate = glm::translate({1}, position);

    auto transform = translate * scale;
    rlPushMatrix();
    {
        rlMultMatrixf(glm::value_ptr(transform));
        DrawModel(m_sphereModel, {0, 0, 0}, 1, color);
    }
    rlPopMatrix();
    */
}

void Renderer::drawParticles() {
    BeginShaderMode(m_shader);
    {
        rlBindShaderBuffer(global.particles.radiusSSBO, 0);
        rlBindShaderBuffer(global.particles.positionSSBO, 1);
        rlBindShaderBuffer(global.particles.colorSSBO, 2);

        float aspect    = (float) GetScreenWidth() / (float) GetScreenHeight();
        auto view       = GetCameraViewMatrix(m_camera);
        auto projection = GetCameraProjectionMatrix(m_camera, aspect);

        SetShaderValueMatrix(m_shader, m_viewLoc, view);
        SetShaderValueMatrix(m_shader, m_projLoc, projection);
        SetShaderValue(m_shader, m_countLoc, &m_particleData->count, SHADER_UNIFORM_INT);

        /*
        rlUpdateShaderBuffer(
                global.particles.positionSSBO,
                &m_particleData->position,
                sizeof(m_particleData->position),
                0
        );

        rlUpdateShaderBuffer(
                global.particles.colorSSBO,
                &m_particleData->color,
                sizeof(m_particleData->color),
                0
        );
        */

        auto& mesh = m_sphereModel.meshes[0];
        rlEnableVertexArray(mesh.vaoId);
        rlDrawVertexArrayInstanced(0, mesh.vertexCount, m_particleData->count);
        rlEnableVertexArray(0);
    }
    EndShaderMode();
}

void Renderer::drawPlane(Plane plane) {
    auto& size     = m_planeData->size[plane];
    auto& normal   = m_planeData->normal[plane];
    auto& position = m_planeData->position[plane];

    auto translate = glm::translate({1}, glm::vec3(position));
    auto rotation  = glm::mat4_cast(glm::rotation({0, 1, 0}, glm::vec3(normal)));
    auto scale     = glm::scale({1}, glm::vec3{size.x, 1, size.y});
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

    auto image   = GenImageColor(1, 1, WHITE);
    auto texture = LoadTextureFromImage(image);

    m_sphereModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
}
