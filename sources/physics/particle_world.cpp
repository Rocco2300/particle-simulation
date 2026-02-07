#include "particle_world.hpp"

#include "objects.hpp"

#include <glad.h>

#include <raylib.h>
#include <rlgl.h>

static constexpr std::array<Color, 7> colors{RED, BLUE, GREEN, YELLOW, MAGENTA, VIOLET, ORANGE};

static glm::vec4 getRandomColor() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 6);

    auto rayColor = colors[dist(gen)];
    return {rayColor.r / 255.0f, rayColor.g / 255.0f, rayColor.b / 255.0f, rayColor.a / 255.0f};
}

ParticleWorld::ParticleWorld() {
    // clang-format off
    global.planes.sizeSSBO = rlLoadShaderBuffer(sizeof(PlaneData::size), nullptr, RL_DYNAMIC_COPY);
    global.planes.normalSSBO = rlLoadShaderBuffer(sizeof(PlaneData::normal), nullptr, RL_DYNAMIC_COPY);
    global.planes.positionSSBO = rlLoadShaderBuffer(sizeof(PlaneData::position), nullptr, RL_DYNAMIC_COPY);

    global.particles.radiusSSBO = rlLoadShaderBuffer(sizeof(ParticleData::radius), nullptr, RL_DYNAMIC_COPY);
    global.particles.positionSSBO = rlLoadShaderBuffer(sizeof(ParticleData::position), nullptr, RL_DYNAMIC_COPY);
    global.particles.velocitySSBO = rlLoadShaderBuffer(sizeof(ParticleData::velocity), nullptr, RL_DYNAMIC_COPY);
    global.particles.accelerationSSBO = rlLoadShaderBuffer(sizeof(ParticleData::acceleration), nullptr, RL_DYNAMIC_COPY);
    global.particles.colorSSBO = rlLoadShaderBuffer(sizeof(ParticleData::color), nullptr, RL_DYNAMIC_COPY);
    // clang-format on
}

void ParticleWorld::spawn(glm::vec3 position, bool randomVelocity) {
    auto velocity = glm::vec3(0);
    auto color    = getRandomColor();

    if (randomVelocity) {
        auto dir = getRandomDirection();
        velocity = glm::normalize(glm::vec3(-1, -1, 1) + dir) * 20.f;
    }

    addParticle(0.125f, position, velocity, color);

    uploadParticleData(m_particleData.count - 1, false);
}

void ParticleWorld::impulse() {
    getData();

    for (int i = 0; i < m_particleData.count; i++) {
        auto vel                   = getRandomDirection() * 100.f;
        m_particleData.velocity[i] = glm::vec4(vel, 0);
    }

    uploadParticleData(0, true);
}

void ParticleWorld::addPlane(glm::vec2 size, glm::vec3 position, glm::vec3 normal) {
    int index = m_planeData.count;

    m_planeData.size[index]     = size;
    m_planeData.position[index] = glm::vec4(position, 0);
    m_planeData.normal[index]   = glm::vec4(normal, 0);

    m_planeData.count++;
}

void ParticleWorld::addParticle(
        float radius,
        glm::vec3 position,
        glm::vec3 velocity,
        glm::vec4 color
) {
    m_particleData.radius[m_particleData.count]   = radius;
    m_particleData.position[m_particleData.count] = glm::vec4(position, 0);
    m_particleData.velocity[m_particleData.count] = glm::vec4(velocity, 0);
    m_particleData.color[m_particleData.count]    = color;

    m_particleData.count++;
}

PlaneData& ParticleWorld::planeData() { return m_planeData; }

ParticleData& ParticleWorld::particleData() { return m_particleData; }

void ParticleWorld::buildBoxWorld() {
    addPlane({10, 10}, {0, 0, 0}, {0, 1, 0});
    addPlane({10, 10}, {0, 10, 0}, {0, -1, 0});
    addPlane({10, 10}, {-5, 5, 0}, {1, 0, 0});
    addPlane({10, 10}, {5, 5, 0}, {-1, 0, 0});
    addPlane({10, 10}, {0, 5, -5}, {0, 0, 1});
    addPlane({10, 10}, {0, 5, 5}, {0, 0, -1});

    constexpr float Offset = 1.f;
    for (float y = 1.0f; y <= 9.0f; y += Offset) {
        for (float x = -4.0f; x <= 4.0f; x += Offset) {
            for (float z = -4.0f; z <= 4.0f; z += Offset) {
                if (m_particleData.count == MaxParticles - 1) {
                    break;
                }

                addParticle(0.125f, {x, y, z}, getRandomDirection() * 20.f, getRandomColor());
            }
        }
    }

    uploadPlaneData(0);
    uploadParticleData(0, false);
}

void ParticleWorld::buildSuperflatWorld() {
    addPlane({100, 100}, {0, 0, 0}, {0, 1, 0});

    constexpr float Offset = 3.f;
    for (float y = 1.0f; y <= 9.0f; y += Offset) {
        for (float x = -4.0f; x <= 4.0f; x += Offset) {
            for (float z = -4.0f; z <= 4.0f; z += Offset) {
                if (m_particleData.count == MaxParticles - 1) {
                    break;
                }

                addParticle(0.125f, {x, y, z}, {0, 0, 0}, getRandomColor());
            }
        }
    }

    uploadPlaneData(0);
    uploadParticleData(0, false);
}

void ParticleWorld::getData() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, global.particles.velocitySSBO);
    glGetBufferSubData(
            GL_SHADER_STORAGE_BUFFER,
            0,
            sizeof(m_particleData.velocity),
            &m_particleData.velocity
    );
}

void ParticleWorld::uploadPlaneData(int offset) {
    // clang-format off
    rlUpdateShaderBuffer(
            global.planes.sizeSSBO,
            m_planeData.size + offset,
            sizeof(m_planeData.size) - offset * sizeof(glm::vec2),
            offset * sizeof(glm::vec2)
    );

    rlUpdateShaderBuffer(
            global.planes.normalSSBO,
            m_planeData.normal + offset,
            sizeof(m_planeData.normal) - offset * sizeof(glm::vec4),
            offset * sizeof(glm::vec4)
    );

    rlUpdateShaderBuffer(
            global.planes.positionSSBO,
            m_planeData.position + offset,
            sizeof(m_planeData.position) - offset * sizeof(glm::vec4),
            offset * sizeof(glm::vec4)
    );
    // clang-format on
}

void ParticleWorld::uploadParticleData(int offset, bool partialUpdate) {
    rlUpdateShaderBuffer(
            global.particles.velocitySSBO,
            m_particleData.velocity + offset,
            sizeof(m_particleData.velocity) - offset * sizeof(glm::vec4),
            offset * sizeof(glm::vec4)
    );

    if (partialUpdate) {
        return;
    }

    rlUpdateShaderBuffer(
            global.particles.positionSSBO,
            m_particleData.position + offset,
            sizeof(m_particleData.position) - offset * sizeof(glm::vec4),
            offset * sizeof(glm::vec4)
    );

    rlUpdateShaderBuffer(
            global.particles.radiusSSBO,
            m_particleData.radius + offset,
            sizeof(m_particleData.radius) - offset * sizeof(float),
            offset * sizeof(float)
    );

    rlUpdateShaderBuffer(
            global.particles.accelerationSSBO,
            m_particleData.acceleration + offset,
            sizeof(m_particleData.acceleration) - offset * sizeof(glm::vec4),
            offset * sizeof(glm::vec4)
    );

    rlUpdateShaderBuffer(
            global.particles.colorSSBO,
            m_particleData.color + offset,
            sizeof(m_particleData.color) - offset * sizeof(glm::vec4),
            offset * sizeof(glm::vec4)
    );
}