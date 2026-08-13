#include "core/rhi/opengl/opengl_pipeline.h"

#include "utils/log.h"




OpenGLPipeline::OpenGLPipeline() {
    glCreateVertexArrays(1, &m_pipelineID);
}


void OpenGLPipeline::bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer){
    ENGINE_LOG_TRACE("OpenGLRHI::Vertex buffer binding...");
    OpenGLBuffer* buffer = static_cast<OpenGLBuffer*>(vertexBuffer.get());

    GLuint attribPos = 0;
    GLuint attribCol = 1;

    GLuint vaoBindingPoint = 0;
    ENGINE_LOG_TRACE("OpenGLRHI::Creating VAO...");
    glVertexArrayVertexBuffer(
        m_pipelineID,
        vaoBindingPoint,
        buffer->getBufferId(),
        0,
        6*sizeof(float));
    ENGINE_LOG_TRACE("OpenGLRHI::VAO Created");
    glEnableVertexArrayAttrib(m_pipelineID, attribPos);
    glEnableVertexArrayAttrib(m_pipelineID, attribCol);

    glVertexArrayAttribFormat(m_pipelineID, attribPos, 3, GL_FLOAT, false, 0);
    glVertexArrayAttribFormat(m_pipelineID, attribCol, 3, GL_FLOAT, false, 0);

    glVertexArrayAttribBinding(m_pipelineID, attribPos, vaoBindingPoint);
    glVertexArrayAttribBinding(m_pipelineID, attribCol, vaoBindingPoint);


}