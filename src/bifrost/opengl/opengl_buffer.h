#pragma once
#include <glad/glad.h>

#include "bifrost/buffer.h"

namespace midgard::bifrost::opengl {

class OpenGLBuffer : public Buffer {
public:
    OpenGLBuffer() = delete;
    OpenGLBuffer(const BufferDesc& desc);
    ~OpenGLBuffer() override;

    void setData(size_t size, const void* data, size_t offset = 0) override;
    BufferType getType() const override { return m_desc.type; }
    size_t getSize() const override { return m_desc.size; }

    GLuint getBufferId() const { return m_bufferID; }

private:
    GLuint m_bufferID = 0;
    BufferDesc m_desc;
};

} // namespace midgard::bifrost::opengl
