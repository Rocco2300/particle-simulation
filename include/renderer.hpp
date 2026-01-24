#pragma once

#include "objects.hpp"

#include <raylib.h>

#include <vector>

class Renderer {
public:
    Renderer();
    Renderer(std::vector<Particle>& particles, std::vector<Plane>& planes);

    void draw();

private:
    Model m_planeModel;
    Model m_sphereModel;

    std::vector<Plane>* m_planes;
    std::vector<Particle>* m_particles;

    void generatePlaneMesh();
    void generateSphereMesh();

    void drawPlane(const Plane& plane);
    void drawParticle(const Particle& particle);
};
