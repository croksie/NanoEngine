#include "rendering/Renderer.h"


void Renderer::Initialize(Window* window) {
    m_rhi = std::make_unique<OpenGLRHI>();
    m_rhi->Initialize(window);

}