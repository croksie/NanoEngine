#include "core/rhi/opengl/opengl_pipeline.h"

#include "utils/log.h"




OpenGLPipeline::OpenGLPipeline(PipelineInfo& info) {
    glCreateVertexArrays(1, &m_vertexArrayID);

    GLuint vertexShader = static_cast<OpenGLShader*>(info.vertexShader.get())->getShaderID();
    GLuint fragmentShader = static_cast<OpenGLShader*>(info.fragmentShader.get())->getShaderID();

    m_shaderProgamID = glCreateProgram();
    glAttachShader(m_shaderProgamID, vertexShader);
    glAttachShader(m_shaderProgamID, fragmentShader);
    glLinkProgram(m_shaderProgamID);
}


void OpenGLPipeline::bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer){
    ENGINE_LOG_TRACE("OpenGLRHI::Vertex buffer binding...");
    OpenGLBuffer* buffer = static_cast<OpenGLBuffer*>(vertexBuffer.get());

    GLuint attribPos = 0;
    GLuint attribCol = 1;

    GLuint vaoBindingPoint = 0;
    ENGINE_LOG_TRACE("OpenGLRHI::Creating VAO...");
    glVertexArrayVertexBuffer(
        m_vertexArrayID,
        vaoBindingPoint,
        buffer->getBufferId(),
        0,
        6*sizeof(float));
    ENGINE_LOG_TRACE("OpenGLRHI::VAO Created");
    glEnableVertexArrayAttrib(m_vertexArrayID, attribPos);
    glEnableVertexArrayAttrib(m_vertexArrayID, attribCol);

    glVertexArrayAttribFormat(m_vertexArrayID, attribPos, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(m_vertexArrayID, attribCol, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));

    glVertexArrayAttribBinding(m_vertexArrayID, attribPos, vaoBindingPoint);
    glVertexArrayAttribBinding(m_vertexArrayID, attribCol, vaoBindingPoint);


}