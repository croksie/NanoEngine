#pragma once
#include "core/rhi/rhi.h"
#include "core/rhi/opengl/opengl_rhi.h"

#include "rendering/mesh.h"

#include <memory>

class Renderer {
public:
    void initialize(Window* window);
    void render();

private:
    std::unique_ptr<RHI> m_rhi;

    Mesh m_mesh;
    std::shared_ptr<Pipeline> m_pipeline;

};