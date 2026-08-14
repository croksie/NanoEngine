#pragma once
#include "core/rhi/rhi_pipeline.h"
#include "core/rhi/rhi_buffer.h"
#include "core/rhi/opengl/opengl_buffer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>

class OpenGLPipeline : public Pipeline{
public:
    OpenGLPipeline() = delete;
    OpenGLPipeline(PipelineInfo& info);
    ~OpenGLPipeline() {}
    void bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer) override;

    GLuint getVertexArrayID() { return m_vertexArrayID; }
    GLuint getShaderProgramID() { return m_shaderProgamID; }

    GLsizei getBindedNumberOfVerticles() { return m_numberOfVerticlesInBindedObject; };

private:
    GLuint m_vertexArrayID;
    GLuint m_shaderProgamID;

    GLsizei m_numberOfVerticlesInBindedObject;

};