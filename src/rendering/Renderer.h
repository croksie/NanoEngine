#pragma once
#include <core/rhi/RHI.h>
#include <core/rhi/OpenGLRHI.h>

#include <memory>

class Renderer {
private:
    std::unique_ptr<RHI> m_rhi;
public:
    void Initialize(Window* window);
};