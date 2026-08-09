#pragma once
#include <core/rhi/rhi.h>
#include <core/rhi/opengl/opengl_rhi.h>

#include <memory>

class Renderer {
private:
    std::unique_ptr<RHI> m_rhi;
public:
    void Initialize(Window* window);
};