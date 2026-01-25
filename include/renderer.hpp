#pragma once

#include "objects.hpp"

#include <raylib.h>

#include <vector>

class Renderer {
public:
    Renderer();
    Renderer(Camera3D& camera, ParticleData& particleData, std::vector<Plane>& planes);

    void draw();

private:
    Shader m_shader;
    int m_viewLoc;
    int m_projLoc;
    int m_countLoc;

    Model m_planeModel;
    Model m_sphereModel;

    Camera3D* m_camera;
    std::vector<Plane>* m_planes;
    ParticleData* m_particleData;

    void generatePlaneMesh();
    void generateSphereMesh();

    void drawPlane(const Plane& plane);
    void drawParticles();
    void drawParticle(Particle particle);
};
