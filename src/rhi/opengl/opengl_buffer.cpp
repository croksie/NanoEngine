#include "rhi/opengl/opengl_buffer.h"



OpenGLBuffer::OpenGLBuffer(const BufferDesc& desc) : m_desc(desc) {
    glCreateBuffers(1, &m_bufferID);
    glNamedBufferStorage(m_bufferID, desc.size, desc.initData, GL_DYNAMIC_STORAGE_BIT);
}

OpenGLBuffer::~OpenGLBuffer() {
    glDeleteBuffers(1, &m_bufferID);
    
}

void OpenGLBuffer::setData(size_t size, const void* data, size_t offset) {
    m_desc.size = size;
    glNamedBufferSubData(m_bufferID, offset, size, data);
}