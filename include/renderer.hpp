#pragma once

#include "objects.hpp"

#include <raylib.h>

#include <vector>

class Renderer {
public:
    Renderer();
    Renderer(ParticleData& particleData, std::vector<Plane>& planes);

    void draw();

private:
    Model m_planeModel;
    Model m_sphereModel;

    std::vector<Plane>* m_planes;
    ParticleData* m_particleData;

    void generatePlaneMesh();
    void generateSphereMesh();

    void drawPlane(const Plane& plane);
    void drawParticle(Particle particle);
};
