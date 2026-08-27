#pragma once
#include <memory>

namespace midgard::bifrost {
    class Buffer;
}

namespace midgard::resource {

class Mesh {
public:
    Mesh() = default;
    Mesh(std::shared_ptr<bifrost::Buffer> vertexBuffer, std::shared_ptr<bifrost::Buffer> indexBuffer) 
        : m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer) {}

    std::shared_ptr<bifrost::Buffer> getVertexBuffer() const { return m_vertexBuffer; }
    std::shared_ptr<bifrost::Buffer> getIndexBuffer() const { return m_indexBuffer; }

private:
    std::shared_ptr<bifrost::Buffer> m_vertexBuffer;
    std::shared_ptr<bifrost::Buffer> m_indexBuffer;
};

} // namespace midgard::resource