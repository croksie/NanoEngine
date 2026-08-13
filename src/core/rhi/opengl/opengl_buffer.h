#pragma once
#include "core/rhi/rhi_buffer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

class OpenGLBuffer : public Buffer {
public:
    OpenGLBuffer(size_t size, const void* data);
    ~OpenGLBuffer() ;

    void setData(size_t size, const void* data, size_t offset = 0) override;
    size_t getSize() override;

    GLuint getBufferId() { return m_bufferID; } 

private :
    GLuint m_bufferID;
    size_t m_size{0};
};