#include "opengl_buffer.h"



OpenGLBuffer::OpenGLBuffer(size_t size, const void* data) : m_size(size) {
    glCreateBuffers(1, &m_bufferID);
    glNamedBufferStorage(m_bufferID, size, data, GL_DYNAMIC_STORAGE_BIT);
}

OpenGLBuffer::~OpenGLBuffer() {
    glDeleteBuffers(1, &m_bufferID);
    
}

void OpenGLBuffer::setData(size_t size, const void* data, size_t offset) {
    m_size = size;
    glNamedBufferSubData(m_bufferID, offset, size, data);
}

size_t OpenGLBuffer::getSize() {
    return m_size;
}  
