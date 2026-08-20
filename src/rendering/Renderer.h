#pragma once
#include "core/rhi/rhi.h"
#include "core/rhi/opengl/opengl_rhi.h"
#include "core/rhi/vulkan/vulkan_rhi.h"

#include "rendering/mesh.h"
#include "rendering/material.h"
#include "rendering/model.h"

#include <memory>
#include <vector>


#include <vulkan/vulkan.h> //To suppress

class Renderer {
public:
    void initialize(Window* window);
    void render();
    void shutdown();


private:
    std::unique_ptr<RHI> m_rhi;

    std::vector<Model> models;

    void createTestModel();

};