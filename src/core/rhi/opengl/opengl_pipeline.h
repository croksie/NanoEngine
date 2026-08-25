#pragma once
#include "core/rhi/rhi.h"
#include "core/rhi/rhi_pipeline.h"
#include "core/rhi/rhi_buffer.h"
#include "core/rhi/opengl/opengl_buffer.h"
#include "core/rhi/opengl/opengl_texture.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>

class OpenGLPipeline : public Pipeline{
public:
    OpenGLPipeline() = delete;
    OpenGLPipeline(PipelineInfo& info);
    ~OpenGLPipeline() {}
    
    void bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer) override;
    void bindInstanceBuffer(std::shared_ptr<Buffer> instanceBuffer) override;
    void bindIndexBuffer(std::shared_ptr<Buffer> indexBuffer) override;
    void bindTexture(std::shared_ptr<Texture> texture, uint32_t slot = 0) override;

    GLuint getVertexArrayID() { return m_vertexArrayID; }
    GLuint getShaderProgramID() { return m_shaderProgamID; }

    GLsizei getBindedNumberOfVerticles() { return m_numberOfVerticlesInBindedObject; };
    GLsizei getBindedNumberOfIndices() { return m_numberOfIndicesInBindedObject; };

private:
    GLuint m_vertexArrayID;
    GLuint m_shaderProgamID;

    GLsizei m_numberOfVerticlesInBindedObject;
    GLsizei m_numberOfIndicesInBindedObject;

};