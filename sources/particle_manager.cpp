#include "particle_manager.hpp"

#include "objects.hpp"

#include <glad.h>
#include <rlgl.h>

static constexpr std::array<Color, 7> colors{RED, BLUE, GREEN, YELLOW, MAGENTA, VIOLET, ORANGE};

static glm::vec4 getRandomColor() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 6);

    auto rayColor = colors[dist(gen)];
    return {rayColor.r / 255.0f, rayColor.g / 255.0f, rayColor.b / 255.0f, rayColor.a / 255.0f};
}

ParticleManager::ParticleManager() {
    constexpr float Offset = 4.f;
    for (float y = 1.0f; y <= 9.0f; y += Offset) {
        for (float x = -4.0f; x <= 4.0f; x += Offset) {
            for (float z = -4.0f; z <= 4.0f; z += Offset) {
                if (m_particleData.count == MaxParticles - 1) {
                    break;
                }

                m_particleData.radius[m_particleData.count]   = 0.125f;
                m_particleData.position[m_particleData.count] = glm::vec4(x, y, z, 0);
                m_particleData.velocity[m_particleData.count] = getRandomDirection() * 20.f;
                m_particleData.color[m_particleData.count]    = getRandomColor();

                m_particleData.count++;
            }
        }
    }

    uploadData(0, false);
}

void ParticleManager::spawn() {
    auto position = glm::vec4(4, 8, -4, 0);
    auto velocity = glm::normalize(glm::vec4(-1, -1, 1, 0) + getRandomVectorOffset()) * 20.f;

    m_particleData.radius[m_particleData.count]   = 0.125f;
    m_particleData.position[m_particleData.count] = position;
    m_particleData.velocity[m_particleData.count] = velocity;
    m_particleData.color[m_particleData.count]    = getRandomColor();

    uploadData(m_particleData.count, false);

    m_particleData.count++;
}

void ParticleManager::impulse() {
    getData();

    for (int i = 0; i < m_particleData.count; i++) {
        m_particleData.velocity[i] = getRandomDirection() * 100.f;
    }

    uploadData(0, true);
}

ParticleData& ParticleManager::particleData() { return m_particleData; }

void ParticleManager::getData() {
    glGetBufferSubData(
            GL_SHADER_STORAGE_BUFFER,
            0,
            sizeof(m_particleData.velocity),
            &m_particleData.velocity
    );
}

void ParticleManager::uploadData(int offset, bool partialUpdate) {
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