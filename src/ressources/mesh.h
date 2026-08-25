#pragma once
#include "rhi/rhi.h"

#include <memory>


class Mesh {
public:
    Mesh() {}
    Mesh(std::shared_ptr<Buffer> vertexBuffer, std::shared_ptr<Buffer> indexBuffer) : m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer) {}

    std::shared_ptr<Buffer> getVertexBuffer() { return m_vertexBuffer; }
    std::shared_ptr<Buffer> getIndexBuffer() { return m_indexBuffer; }


private:
    std::shared_ptr<Buffer> m_vertexBuffer;
    std::shared_ptr<Buffer> m_indexBuffer;
};