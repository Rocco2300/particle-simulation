#pragma once

#include "objects.hpp"

#include <raylib.h>

#include <vector>

 struct RendererContext {
     SimulationMode simulationMode;

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

    Model m_planeModel;
    Model m_sphereModel;

    Camera3D* m_camera;
    PlaneData* m_planeData;
    ParticleData* m_particleData;
    SimulationMode m_simulationMode;

    void generatePlaneMesh();
    void generateSphereMesh();

    void drawParticles();
    void drawPlane(Plane plane);
    void drawParticle(Particle particle);
};
