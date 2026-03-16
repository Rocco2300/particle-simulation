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

Renderer::Renderer(RendererContext& rendererContext)
    : m_camera{rendererContext.camera}
    , m_planeData{rendererContext.planeData}
    , m_particleData{rendererContext.particleData}
    , m_simulationMode{rendererContext.simulationMode}
    , m_projectPath{rendererContext.projectPath} {

    generateCubeMesh();
    generatePlaneMesh();
    generateSphereMesh();

    auto shadersPath        = m_projectPath / "shaders";
    auto vertexShaderPath   = shadersPath / "particle.vert";
    auto fragmentShaderPath = shadersPath / "particle.frag";
    m_shader = LoadShader(vertexShaderPath.string().c_str(), fragmentShaderPath.string().c_str());

    m_viewLoc          = GetShaderLocation(m_shader, "view");
    m_projLoc          = GetShaderLocation(m_shader, "projection");
    m_countLoc         = GetShaderLocation(m_shader, "count");
    m_renderingTypeLoc = GetShaderLocation(m_shader, "renderingType");
}

void Renderer::draw() {
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(*m_camera);
        {
            //DrawModel(m_cubeModel, {0, 0, 0}, 1, RED);
            if (m_planeData) {
                for (int plane = 0; plane < m_planeData->count; plane++) {
                    drawPlane(plane);
                }
            }

            drawParticles();

            DrawGrid(10, 1.f);
        }
        EndMode3D();

        DrawFPS(10, 10);
        std::string particleNo = std::to_string(m_particleData->count);
        DrawText(particleNo.c_str(), 10, 36, 24, DARKGREEN);
    }
    EndDrawing();
}

void Renderer::drawParticles() {
    BeginShaderMode(m_shader);
    {
        rlBindShaderBuffer(global.particles.typeSSBO, 0);
        rlBindShaderBuffer(global.particles.radiusSSBO, 1);
        rlBindShaderBuffer(global.particles.positionSSBO, 2);
        rlBindShaderBuffer(global.particles.colorSSBO, 3);

        float aspect    = (float) GetScreenWidth() / (float) GetScreenHeight();
        auto view       = GetCameraViewMatrix(m_camera);
        auto projection = GetCameraProjectionMatrix(m_camera, aspect);

        SetShaderValueMatrix(m_shader, m_viewLoc, view);
        SetShaderValueMatrix(m_shader, m_projLoc, projection);
        SetShaderValue(m_shader, m_countLoc, &m_particleData->count, SHADER_UNIFORM_INT);

        if (m_simulationMode == SimulationMode::CPU) {
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
        }

        int sphereRenderingType = SPHERE_TYPE;
        SetShaderValue(m_shader, m_renderingTypeLoc, &sphereRenderingType, SHADER_UNIFORM_INT);
        auto mesh = m_sphereModel.meshes[0];
        rlEnableVertexArray(mesh.vaoId);
        rlDrawVertexArrayInstanced(0, mesh.vertexCount, m_particleData->count);
        //rlDrawVertexArrayElementsInstanced(0, mesh.triangleCount * 3, nullptr, m_particleData->count);
        rlEnableVertexArray(0);

        int boxRenderingType = BOX_TYPE;
        SetShaderValue(m_shader, m_renderingTypeLoc, &boxRenderingType, SHADER_UNIFORM_INT);
        mesh = m_cubeModel.meshes[0];
        rlEnableVertexArray(mesh.vaoId);
        rlDrawVertexArrayElementsInstanced(0, mesh.triangleCount * 3, nullptr, m_particleData->count);
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

void Renderer::generateCubeMesh() {
    Mesh mesh   = GenMeshCube(2, 2, 2);
    m_cubeModel = LoadModelFromMesh(mesh);

    auto image   = GenImageColor(1, 1, WHITE);
    auto texture = LoadTextureFromImage(image);

    m_cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
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
