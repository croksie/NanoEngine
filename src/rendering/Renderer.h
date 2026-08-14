#pragma once
#include "core/rhi/rhi.h"
#include "core/rhi/opengl/opengl_rhi.h"

#include "rendering/mesh.h"
#include "rendering/material.h"
#include "rendering/model.h"

#include <memory>
#include <vector>

class Renderer {
public:
    void initialize(Window* window);
    void render();

private:
    std::unique_ptr<RHI> m_rhi;

    std::vector<Model> models;

    void createTestModel();

};