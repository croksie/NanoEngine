#pragma once
#include "core/rhi/rhi_buffer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

class OpenGLBuffer : public Buffer {
public:
    OpenGLBuffer() = delete;
    OpenGLBuffer(const BufferDesc& desc);
    ~OpenGLBuffer() ;

    void setData(size_t size, const void* data, size_t offset = 0) override;
    BufferType getType() const override { return m_desc.type; }
    size_t getSize() const override { return m_desc.size; }

    GLuint getBufferId() { return m_bufferID; } 

private :
    GLuint m_bufferID;
    BufferDesc m_desc;
};