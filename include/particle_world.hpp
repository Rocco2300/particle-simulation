#pragma once

#include "objects.hpp"

#include <random>

static glm::vec3 getRandomVectorOffset() {
    static std::mt19937 gen(std::random_device{}());
    static std::normal_distribution<float> dist(-1.f, 1.f);

    return glm::vec3(dist(gen), dist(gen), dist(gen));
}

static glm::vec3 getRandomDirection() {
    constexpr std::array<glm::vec3, 8> directions{
            {{1, 1, 1},
             {1, 1, -1},
             {-1, 1, 1},
             {-1, 1, -1}}
    };

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution dist(0, 3);

    return glm::normalize(directions[dist(gen)]);
}

class ParticleWorld {
public:
    ParticleWorld();

    void impulse();
    void spawn(glm::vec3 position, bool randomVelocity = false);

    void addPlane(glm::vec2 size, glm::vec3 position, glm::vec3 normal);
    void addParticle(float radius, glm::vec3 position, glm::vec3 velocity, glm::vec4 color);

    void buildBoxWorld(int particleNo = MaxParticles);
    void buildSuperflatWorld(int particleNo = MaxParticles);

    PlaneData& planeData();
    ParticleData& particleData();

private:
    PlaneData m_planeData;
    ParticleData m_particleData;

    void getData();

    void uploadPlaneData(int offset = 0);
    void uploadParticleData(int offset = 0, bool partialUpdate = false);
};