#include "simulation.hpp"

#include <glad.h>
#include <iostream>
#include <rlgl.h>

Simulation::Simulation(
        SimulationMode simulationMode,
        ParticleData& particleData,
        PlaneData& planeData
)
    : m_planeData{&planeData}
    , m_particleData{&particleData}
    , m_simulationMode{simulationMode} {

    char* applyCode =
            LoadFileText("C:/Users/grigo/repos/particle-simulation/shaders/applyForces.comp");
    uint32_t applyShader = rlCompileShader(applyCode, RL_COMPUTE_SHADER);
    m_applyProgram       = rlLoadComputeShaderProgram(applyShader);
    UnloadFileText(applyCode);

    char* moveCode =
            LoadFileText("C:/Users/grigo/repos/particle-simulation/shaders/moveParticles.comp");
    uint32_t moveShader = rlCompileShader(moveCode, RL_COMPUTE_SHADER);
    m_moveProgram       = rlLoadComputeShaderProgram(moveShader);
    UnloadFileText(moveCode);

    char* planeCollisionCode = LoadFileText(
            "C:/Users/grigo/repos/particle-simulation/shaders/resolvePlaneCollisions.comp"
    );
    uint32_t planeCollisionsShader = rlCompileShader(planeCollisionCode, RL_COMPUTE_SHADER);
    m_planeCollisionsProgram       = rlLoadComputeShaderProgram(planeCollisionsShader);
    UnloadFileText(planeCollisionCode);

    char* particleCollisionCode = LoadFileText(
            "C:/Users/grigo/repos/particle-simulation/shaders/resolveParticleCollisions.comp"
    );
    uint32_t particleCollisionsShader = rlCompileShader(particleCollisionCode, RL_COMPUTE_SHADER);
    m_particleCollisionsProgram       = rlLoadComputeShaderProgram(particleCollisionsShader);
    UnloadFileText(particleCollisionCode);
}

void Simulation::update(float deltaTime) {
    if (m_simulationMode == SimulationMode::GPU) {
        const int steps   = 3;
        auto subDeltaTime = deltaTime / steps;
        for (int i = 0; i < steps; i++) {
            gpuApplyForces(subDeltaTime);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            gpuMoveParticles(subDeltaTime);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            gpuResolvePlaneCollisions();
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            gpuResolveParticleCollisions();
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
    } else {
        const int steps   = 10;
        auto subDeltaTime = deltaTime / steps;
        for (int i = 1; i <= steps; i++) {
            for (int particle = 0; particle < m_particleData->count; particle++) {
                applyForces(particle, subDeltaTime);
                //clampVelocity(particle);
                moveParticle(particle, subDeltaTime);
                resolveCollisions(particle);
            }
        }
    }
}

void Simulation::applyForces(Particle particle, float deltaTime) {
    const auto gravity = glm::vec4(0, -1, 0, 0) * 9.81f;

    auto& velocity     = m_particleData->velocity[particle];
    auto& acceleration = m_particleData->acceleration[particle];

    acceleration += gravity * deltaTime;
    velocity += acceleration * deltaTime;
}

void Simulation::clampVelocity(Particle particle) {
    auto& velocity = m_particleData->velocity[particle];

    if (velocity.length() >= 25.f) {
        velocity = glm::normalize(velocity) * 16.f;
    }
}

void Simulation::moveParticle(Particle particle, float deltaTime) {
    auto& position = m_particleData->position[particle];
    auto& velocity = m_particleData->velocity[particle];

    position += velocity * deltaTime;
}

float Simulation::getSignedDistance(Particle particle, Plane plane) {
    auto& position      = m_particleData->position[particle];
    auto& planeNormal   = m_planeData->normal[plane];
    auto& planePosition = m_planeData->position[plane];

    return glm::dot(planeNormal, position - planePosition);
}

bool Simulation::isCollidingPlane(Particle particle, Plane plane) {
    auto& radius = m_particleData->radius[particle];

    return getSignedDistance(particle, plane) < radius;
}

void Simulation::solvePlanePenetration(Particle particle, Plane plane) {
    auto& radius      = m_particleData->radius[particle];
    auto& position    = m_particleData->position[particle];
    auto& planeNormal = m_planeData->normal[plane];

    auto distance   = getSignedDistance(particle, plane);
    auto moveAmount = radius - distance + 0.0001f;
    position += planeNormal * moveAmount;
}

bool Simulation::isCollidingParticle(Particle p1, Particle p2) {
    auto& rad1 = m_particleData->radius[p1];
    auto& rad2 = m_particleData->radius[p2];
    auto& pos1 = m_particleData->position[p1];
    auto& pos2 = m_particleData->position[p2];

    auto distance = glm::distance(pos1, pos2);

    if (distance < rad1 + rad2) {
        return true;
    }

    return false;
}

void Simulation::solveParticlePenetration(Particle p1, Particle p2) {
    auto& rad1 = m_particleData->radius[p1];
    auto& rad2 = m_particleData->radius[p2];
    auto& pos1 = m_particleData->position[p1];
    auto& pos2 = m_particleData->position[p2];

    auto normal     = glm::normalize(pos2 - pos1);
    auto distance   = glm::distance(pos1, pos2);
    auto moveAmount = rad1 + rad2 - distance;

    pos1 += normal * -((moveAmount / 2.0f) + 0.0001f);
    pos2 += normal * ((moveAmount / 2.0f) + 0.0001f);
}

void Simulation::resolveCollisions(Particle particle) {
    auto& pos1 = m_particleData->position[particle];
    auto& vel1 = m_particleData->velocity[particle];

    for (int i = 0; i < m_planeData->count; i++) {
        if (isCollidingPlane(particle, i)) {
            solvePlanePenetration(particle, i);

            auto& normal = m_planeData->normal[i];
            vel1         = glm::reflect(vel1, normal) * 0.90f;
        }
    }

    for (int i = 0; i < m_particleData->count; i++) {
        auto& pos2 = m_particleData->position[i];
        auto& vel2 = m_particleData->velocity[i];

        if (i != particle && isCollidingParticle(i, particle)) {
            auto normal   = glm::normalize(pos2 - pos1);
            auto distance = glm::distance(pos1, pos2);

            solveParticlePenetration(i, particle);

            vel1 = glm::reflect(vel1, normal) * 0.95f;
            vel2 = glm::reflect(vel2, normal) * 0.95f;
        }
    }
}

void Simulation::gpuApplyForces(float deltaTime) {
    rlEnableShader(m_applyProgram);

    auto deltaTimeLoc = rlGetLocationUniform(m_applyProgram, "deltaTime");

    rlBindShaderBuffer(global.particles.velocitySSBO, 0);
    rlBindShaderBuffer(global.particles.accelerationSSBO, 1);

    rlSetUniform(deltaTimeLoc, &deltaTime, SHADER_UNIFORM_FLOAT, 1);

    rlComputeShaderDispatch(m_particleData->count, 1, 1);
    rlDisableShader();
}

void Simulation::gpuMoveParticles(float deltaTime) {
    rlEnableShader(m_moveProgram);

    auto deltaTimeLoc = rlGetLocationUniform(m_moveProgram, "deltaTime");

    rlBindShaderBuffer(global.particles.positionSSBO, 0);
    rlBindShaderBuffer(global.particles.velocitySSBO, 1);

    rlSetUniform(deltaTimeLoc, &deltaTime, SHADER_UNIFORM_FLOAT, 1);

    rlComputeShaderDispatch(m_particleData->count, 1, 1);
    rlDisableShader();
}

void Simulation::gpuResolvePlaneCollisions() {
    rlEnableShader(m_planeCollisionsProgram);

    rlBindShaderBuffer(global.particles.radiusSSBO, 0);
    rlBindShaderBuffer(global.particles.positionSSBO, 1);
    rlBindShaderBuffer(global.particles.velocitySSBO, 2);
    rlBindShaderBuffer(global.planes.sizeSSBO, 3);
    rlBindShaderBuffer(global.planes.normalSSBO, 4);
    rlBindShaderBuffer(global.planes.positionSSBO, 5);

    rlComputeShaderDispatch(m_particleData->count, 1, 1);
    rlDisableShader();
}

void Simulation::gpuResolveParticleCollisions() {
    rlEnableShader(m_particleCollisionsProgram);

    auto particleCountLoc = rlGetLocationUniform(m_particleCollisionsProgram, "particleCount");

    rlBindShaderBuffer(global.particles.radiusSSBO, 0);
    rlBindShaderBuffer(global.particles.positionSSBO, 1);
    rlBindShaderBuffer(global.particles.velocitySSBO, 2);

    rlSetUniform(particleCountLoc, &m_particleData->count, SHADER_UNIFORM_INT, 1);

    rlComputeShaderDispatch(m_particleData->count, 1, 1);
    rlDisableShader();
}