#pragma once

#include "objects.hpp"

#include <raylib.h>

#include <filesystem>

 struct RendererContext {
     SimulationMode simulationMode;
     std::filesystem::path projectPath;

     Camera3D* camera;
     PlaneData* planeData;
     ParticleData* particleData;
 };

class Renderer {
public:
    Renderer();
    Renderer(RendererContext& renderContext);

    void draw();

private:
    Shader m_shader;
    int m_viewLoc;
    int m_projLoc;
    int m_countLoc;
    int m_renderingTypeLoc;

    Model m_cubeModel;
    Model m_planeModel;
    Model m_sphereModel;

    Camera3D* m_camera;
    PlaneData* m_planeData;
    ParticleData* m_particleData;

    SimulationMode m_simulationMode;
    std::filesystem::path m_projectPath;

    void generateCubeMesh();
    void generatePlaneMesh();
    void generateSphereMesh();

    void drawParticles();
    void drawPlane(Plane plane);
};
