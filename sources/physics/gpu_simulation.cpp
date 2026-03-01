#include "gpu_simulation.hpp"
#include <glad.h>

#include <raylib.h>
#include <rlgl.h>

GPUSimulation::GPUSimulation(SimulationContext& simulationContext)
    : Simulation(simulationContext) {
    char* applyCode =
            LoadFileText("C:/Users/grigo/repos/particle-simulation/shaders/applyForces.comp");
    uint32_t applyShader = rlCompileShader(applyCode, RL_COMPUTE_SHADER);
    m_applyProgram       = rlLoadComputeShaderProgram(applyShader);
    UnloadFileText(applyCode);

    char* clampCode =
            LoadFileText("C:/Users/grigo/repos/particle-simulation/shaders/clampVelocity.comp");
    uint32_t clampShader = rlCompileShader(clampCode, RL_COMPUTE_SHADER);
    m_clampProgram       = rlLoadComputeShaderProgram(clampShader);
    UnloadFileText(clampCode);

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

    char* populateGridCode =
            LoadFileText("C:/Users/grigo/Repos/particle-simulation/shaders/populateGrid.comp");
    uint32_t populateGridShader = rlCompileShader(populateGridCode, RL_COMPUTE_SHADER);
    m_populateGridProgram       = rlLoadComputeShaderProgram(populateGridShader);
    UnloadFileText(populateGridCode);

    char* clearGridCode =
            LoadFileText("C:/Users/grigo/Repos/particle-simulation/shaders/clearGrid.comp");
    uint32_t clearGridShader = rlCompileShader(clearGridCode, RL_COMPUTE_SHADER);
    m_clearGridProgram       = rlLoadComputeShaderProgram(clearGridShader);
    UnloadFileText(clearGridCode);
}

void GPUSimulation::update(float deltaTime) {
    auto subDeltaTime = deltaTime / m_steps;
    for (int i = 0; i < m_steps; i++) {
        if (m_gravity) {
            gpuApplyForces(subDeltaTime);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        if (m_spatialPartition) {
            gpuClearGrid();
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        gpuClampVelocities();
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        gpuMoveParticles(subDeltaTime);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        if (m_planeCollisions) {
            gpuResolvePlaneCollisions();
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        // TODO: low prio - might cause issues on superflat world
        // particles getting out of the bounds of the grid
        if (m_spatialPartition && m_planeCollisions) {
            gpuPopulateGrid();
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        gpuResolveParticleCollisions();
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

int GPUSimulation::getInvocationCount() const {
    return std::ceil(static_cast<float>(m_particleData->count) / 32.f);
}

void GPUSimulation::gpuPopulateGrid() {
    rlEnableShader(m_populateGridProgram);

    rlBindShaderBuffer(global.particles.positionSSBO, 0);
    rlBindShaderBuffer(global.particles.gridSSBO, 1);

    constexpr int GridSize        = 12 * 12 * 12;
    constexpr int InvocationCount = GridSize / 32;
    rlComputeShaderDispatch(InvocationCount, 1, 1);
    rlDisableShader();
}

void GPUSimulation::gpuClearGrid() {
    rlEnableShader(m_clearGridProgram);

    rlBindShaderBuffer(global.particles.gridSSBO, 0);

    constexpr int GridSize        = 12 * 12 * 12;
    constexpr int InvocationCount = GridSize / 32;
    rlComputeShaderDispatch(InvocationCount, 1, 1);
    rlDisableShader();
}

void GPUSimulation::gpuApplyForces(float deltaTime) {
    rlEnableShader(m_applyProgram);

    auto deltaTimeLoc = rlGetLocationUniform(m_applyProgram, "deltaTime");

    rlBindShaderBuffer(global.particles.velocitySSBO, 0);
    rlBindShaderBuffer(global.particles.accelerationSSBO, 1);

    rlSetUniform(deltaTimeLoc, &deltaTime, SHADER_UNIFORM_FLOAT, 1);

    rlComputeShaderDispatch(getInvocationCount(), 1, 1);
    rlDisableShader();
}

void GPUSimulation::gpuClampVelocities() {
    rlEnableShader(m_clampProgram);

    rlBindShaderBuffer(global.particles.velocitySSBO, 0);

    rlComputeShaderDispatch(getInvocationCount(), 1, 1);
    rlDisableShader();
}

void GPUSimulation::gpuMoveParticles(float deltaTime) {
    rlEnableShader(m_moveProgram);

    auto deltaTimeLoc = rlGetLocationUniform(m_moveProgram, "deltaTime");

    rlBindShaderBuffer(global.particles.positionSSBO, 0);
    rlBindShaderBuffer(global.particles.velocitySSBO, 1);

    rlSetUniform(deltaTimeLoc, &deltaTime, SHADER_UNIFORM_FLOAT, 1);

    rlComputeShaderDispatch(getInvocationCount(), 1, 1);
    rlDisableShader();
}

void GPUSimulation::gpuResolvePlaneCollisions() {
    rlEnableShader(m_planeCollisionsProgram);

    auto planeCountLoc = rlGetLocationUniform(m_planeCollisionsProgram, "planeCount");

    rlBindShaderBuffer(global.particles.radiusSSBO, 0);
    rlBindShaderBuffer(global.particles.positionSSBO, 1);
    rlBindShaderBuffer(global.particles.velocitySSBO, 2);
    rlBindShaderBuffer(global.planes.sizeSSBO, 3);
    rlBindShaderBuffer(global.planes.normalSSBO, 4);
    rlBindShaderBuffer(global.planes.positionSSBO, 5);

    rlSetUniform(planeCountLoc, &m_planeData->count, SHADER_UNIFORM_INT, 1);

    rlComputeShaderDispatch(getInvocationCount(), 1, 1);
    rlDisableShader();
}

void GPUSimulation::gpuResolveParticleCollisions() {
    rlEnableShader(m_particleCollisionsProgram);

    auto partitionLoc     = rlGetLocationUniform(m_particleCollisionsProgram, "partitionSpace");
    auto particleCountLoc = rlGetLocationUniform(m_particleCollisionsProgram, "particleCount");

    rlBindShaderBuffer(global.particles.radiusSSBO, 0);
    rlBindShaderBuffer(global.particles.positionSSBO, 1);
    rlBindShaderBuffer(global.particles.velocitySSBO, 2);
    rlBindShaderBuffer(global.particles.gridSSBO, 3);

    int spatialPartition = m_spatialPartition;
    rlSetUniform(partitionLoc, &spatialPartition, SHADER_UNIFORM_INT, 1);
    rlSetUniform(particleCountLoc, &m_particleData->count, SHADER_UNIFORM_INT, 1);

    rlComputeShaderDispatch(getInvocationCount(), 1, 1);
    rlDisableShader();
}
