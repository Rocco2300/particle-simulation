#pragma once

#include "objects.hpp"

#include <random>

static glm::vec4 getRandomVectorOffset() {
    static std::mt19937 gen(std::random_device{}());
    static std::normal_distribution<float> dist(-1.f, 1.f);

    return glm::vec4(dist(gen), dist(gen), dist(gen), 0);
}

static glm::vec4 getRandomDirection() {
    constexpr std::array<glm::vec4, 8> directions{
            {{1, 1, 1, 0},
             {1, 1, -1, 0},
             {1, -1, 1, 0},
             {1, -1, -1, 0},
             {-1, 1, 1, 0},
             {-1, 1, -1, 0},
             {-1, -1, 1, 0},
             {-1, -1, -1, 0}}
    };

    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 7);

    auto direction = directions[dist(gen)];
    direction += getRandomVectorOffset();

    return glm::normalize(direction);
}

class ParticleManager {
public:
    ParticleManager();

    void spawn();
    void impulse();

    ParticleData& particleData();

private:
    ParticleData m_particleData;

    void getData();
    void uploadData(int offset = 0, bool partialUpdate = false);
};