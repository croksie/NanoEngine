#pragma once
#include "rhi/rhi.h"
#include "rhi/opengl/opengl_rhi.h"
#include "rhi/vulkan/vulkan_rhi.h"
#include "core/config.h"

#include "ressources/mesh.h"
#include "ressources/material.h"
#include "scene/model.h"
#include "scene/camera.h"
#include "platform/input/input.h"

#include <memory>
#include <vector>

class Renderer {
public:
    void initialize(Window* window, std::shared_ptr<EngineConfig> config);
    void render();
    void shutdown();


private:
    std::shared_ptr<EngineConfig> m_config;
    std::unique_ptr<RHI> m_rhi;

    Camera m_camera;

    std::vector<Model> models;

    std::vector<InstanceData> m_instances = std::vector<InstanceData>(900);
    std::shared_ptr<Buffer> m_instanceBuffer;
    std::shared_ptr<Texture> m_texture;
    void createTestModel();

    float m_lastFrameTime = 0.0f;
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    bool m_firstMouse = true;
};