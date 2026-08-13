#include "rendering/renderer.h"

#include "utils/log.h"


void Renderer::initialize(Window* window) {
    ENGINE_LOG_INFO("Renderer initializing ...");

    float vertices[] = {
    -1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    m_rhi = std::make_unique<OpenGLRHI>();
    m_rhi->initialize(window);
    std::shared_ptr<Buffer> buffer = m_rhi->createBuffer(vertices, sizeof(vertices));
    m_pipeline = m_rhi->createPipeline();
    ENGINE_LOG_DEBUG("Is pipeline valid ? : {}", m_pipeline != nullptr ? "true" : "false");

    m_mesh = Mesh(buffer);
    ENGINE_LOG_INFO("Renderer initialized");
}
void Renderer::render() {
    ENGINE_LOG_DEBUG("Render start");
    m_rhi->beginFrame();
    m_rhi->clear();
    m_rhi->bindPipeline();
    m_rhi->bindVertexBuffer(m_pipeline, m_mesh.getVertexBuffer());
    m_rhi->draw(m_pipeline);


    m_rhi->endFrame();
    ENGINE_LOG_DEBUG("Render end");
}