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

    if(buffer->getType() != BufferType::VERTEX) {
        ENGINE_LOG_ERROR("OpenGLRHI::Buffer is not of type VERTEX");
        return;
    }

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

    m_numberOfVerticlesInBindedObject = static_cast<GLsizei>(buffer->getSize() / (6 * sizeof(float)));
    ENGINE_LOG_TRACE("OpenGLRHI::VAO Created");
    glEnableVertexArrayAttrib(m_vertexArrayID, attribPos);
    glEnableVertexArrayAttrib(m_vertexArrayID, attribCol);

    glVertexArrayAttribFormat(m_vertexArrayID, attribPos, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(m_vertexArrayID, attribCol, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));

    glVertexArrayAttribBinding(m_vertexArrayID, attribPos, vaoBindingPoint);
    glVertexArrayAttribBinding(m_vertexArrayID, attribCol, vaoBindingPoint);
}

void OpenGLPipeline::bindInstanceBuffer(std::shared_ptr<Buffer> instanceBuffer) {
    ENGINE_LOG_TRACE("OpenGLRHI::Instance buffer binding...");
    OpenGLBuffer* buffer = static_cast<OpenGLBuffer*>(instanceBuffer.get());

    if(buffer->getType() != BufferType::VERTEX) {
        ENGINE_LOG_ERROR("OpenGLRHI::Buffer is not of type VERTEX");
        return;
    }

    GLuint vaoBindingPoint = 1;
    glVertexArrayVertexBuffer(
        m_vertexArrayID,
        vaoBindingPoint,
        buffer->getBufferId(),
        0,
        sizeof(InstanceData));

    glVertexArrayBindingDivisor(m_vertexArrayID, vaoBindingPoint, 1);

    for (GLuint i = 0; i < 4; ++i) {
        GLuint attribLocation = 2 + i;
        glEnableVertexArrayAttrib(m_vertexArrayID, attribLocation);
        glVertexArrayAttribFormat(m_vertexArrayID, attribLocation, 4, GL_FLOAT, GL_FALSE, i * sizeof(glm::vec4));
        glVertexArrayAttribBinding(m_vertexArrayID, attribLocation, vaoBindingPoint);
    }
}

void OpenGLPipeline::bindIndexBuffer(std::shared_ptr<Buffer> indexBuffer) {
    ENGINE_LOG_TRACE("OpenGLRHI::Index buffer binding...");
    OpenGLBuffer* buffer = static_cast<OpenGLBuffer*>(indexBuffer.get());

    if (buffer->getType() != BufferType::INDEX) {
        ENGINE_LOG_ERROR("OpenGLRHI::Buffer is not of type INDEX");
        return;
    }

    glVertexArrayElementBuffer(m_vertexArrayID, buffer->getBufferId());
    m_numberOfIndicesInBindedObject = static_cast<GLsizei>(buffer->getSize() / sizeof(uint32_t));
}
