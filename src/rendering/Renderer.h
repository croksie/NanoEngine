#pragma once
#include "core/rhi/rhi.h"
#include "core/rhi/opengl/opengl_rhi.h"
#include "core/rhi/vulkan/vulkan_rhi.h"
#include "core/config.h"

#include "rendering/mesh.h"
#include "rendering/material.h"
#include "rendering/model.h"

#include <memory>
#include <vector>


#include <vulkan/vulkan.h> //To suppress

class Renderer {
public:
    void initialize(Window* window, std::shared_ptr<EngineConfig> config);
    void render();
    void shutdown();


private:
    std::shared_ptr<EngineConfig> m_config;
    std::unique_ptr<RHI> m_rhi;

    std::vector<Model> models;

    std::vector<InstanceData> m_instances = std::vector<InstanceData>(900);
    std::shared_ptr<Buffer> m_instanceBuffer;
    void createTestModel();
};