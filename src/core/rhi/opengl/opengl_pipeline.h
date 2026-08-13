#pragma once
#include "core/rhi/rhi_pipeline.h"
#include "core/rhi/rhi_buffer.h"
#include "core/rhi/opengl/opengl_buffer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>

class OpenGLPipeline : public Pipeline{
public:
    OpenGLPipeline();
    ~OpenGLPipeline() {}
    void bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer) override;

    GLuint getPipelineID() { return m_pipelineID; }


private:
    GLuint m_pipelineID;

};